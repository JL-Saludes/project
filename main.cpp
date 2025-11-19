// Encrypted Notepad
// Build: g++ main.cpp -o notepad -lssl -lcrypto

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <termios.h>
#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

static void handleErrors(const std::string &msg = "Error") {
    std::cerr << msg << "\n";
    ERR_print_errors_fp(stderr);
    exit(1);
}

std::string read_password(const char* prompt) {
    std::string password;
    struct termios oldt{}, newt{};

    std::cout << prompt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << '\n';
    return password;
}

bool encrypt(const std::string &plaintext,
             const std::string &password,
             std::vector<unsigned char> &out)
{
    const char header[] = "NOTEPADv1";     // 9 bytes
    const size_t HEADER_LEN = 9;
    const int SALT_SIZE = 16;

    unsigned char salt[SALT_SIZE];
    if (RAND_bytes(salt, SALT_SIZE) != 1)
        handleErrors("Failed to generate salt");

    const int KEY_IV_LEN = 48; // 32 key + 16 IV
    unsigned char keyiv[KEY_IV_LEN];

    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                          salt, SALT_SIZE,
                          10000, EVP_sha256(),
                          KEY_IV_LEN, keyiv) != 1)
        handleErrors("PBKDF2 failed");

    unsigned char key[32], iv[16];
    memcpy(key, keyiv, 32);
    memcpy(iv, keyiv + 32, 16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors("Ctx alloc failed");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1)
        handleErrors("EncryptInit failed");

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    int len = 0, total = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.data()),
                          plaintext.size()) != 1)
        handleErrors("EncryptUpdate failed");

    total = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
        handleErrors("EncryptFinal failed");

    total += len;

    EVP_CIPHER_CTX_free(ctx);

    // Output format: header (9) + salt (16) + ciphertext
    out.clear();
    out.insert(out.end(), header, header + HEADER_LEN);
    out.insert(out.end(), salt, salt + SALT_SIZE);
    out.insert(out.end(), ciphertext.begin(), ciphertext.begin() + total);

    return true;
}

bool decrypt(const std::vector<unsigned char> &in,
             const std::string &password,
             std::string &out_plain)
{
    const char header[] = "NOTEPADv1";
    const size_t HEADER_LEN = 9;
    const int SALT_SIZE = 16;

    if (in.size() < HEADER_LEN + SALT_SIZE)
        return false;

    if (memcmp(in.data(), header, HEADER_LEN) != 0)
        return false;

    const unsigned char *salt = in.data() + HEADER_LEN;
    const unsigned char *ciphertext = in.data() + HEADER_LEN + SALT_SIZE;
    size_t ciphertext_len = in.size() - HEADER_LEN - SALT_SIZE;

    unsigned char keyiv[48];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                          salt, SALT_SIZE,
                          10000, EVP_sha256(),
                          48, keyiv) != 1)
        return false;

    unsigned char key[32], iv[16];
    memcpy(key, keyiv, 32);
    memcpy(iv, keyiv + 32, 16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors("Ctx alloc failed");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> plaintext(ciphertext_len + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0, total = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext, ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    total = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false; // wrong password or corrupt ciphertext
    }

    total += len;

    EVP_CIPHER_CTX_free(ctx);

    out_plain.assign(reinterpret_cast<char*>(plaintext.data()), total);
    return true;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cout << "Usage:\n"
                  << "  " << argv[0] << " save <file>   # save encrypted note\n"
                  << "  " << argv[0] << " read <file>   # read encrypted note\n";
        return 1;
    }

    std::string cmd = argv[1];
    std::string path = argv[2];

    if (cmd == "save") {

        std::cout << "Enter note (Ctrl+D to finish):\n";
        std::string note, line;
        while (std::getline(std::cin, line)) {
            note += line;
            if (!std::cin.eof()) note += '\n';
        }

        std::string password = read_password("Password: ");

        std::vector<unsigned char> out;
        if (!encrypt(note, password, out)) {
            std::cerr << "Encryption failed\n";
            return 1;
        }

        std::ofstream ofs(path, std::ios::binary);
        if (!ofs) {
            std::cerr << "Cannot write file\n";
            return 1;
        }

        ofs.write(reinterpret_cast<const char*>(out.data()), out.size());
        std::cout << "Saved encrypted note to " << path << "\n";

    } else if (cmd == "read") {

        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) {
            std::cerr << "Cannot open file\n";
            return 1;
        }

        std::vector<unsigned char> in(
            (std::istreambuf_iterator<char>(ifs)),
            std::istreambuf_iterator<char>()
        );

        std::string password = read_password("Password: ");
        std::string plain;

        if (!decrypt(in, password, plain)) {
            std::cerr << "Decryption failed (wrong password or corrupt file)\n";
            return 2;
        }

        std::cout << "---- SECRET NOTE ----\n";
        std::cout << plain << "\n";

    } else {
        std::cerr << "Unknown command\n";
        return 1;
    }

    return 0;
}
