Encrypted Notepad
=================

This repository contains a small CLI encrypted notepad written in C++ (`main.cpp`). It saves and reads a secret note encrypted with AES-256-CBC using a password-derived key (PBKDF2 + SHA-256) and a random salt.

Build
-----

On Debian/Ubuntu, install the OpenSSL development headers if missing:

```bash
sudo apt-get update
sudo apt-get install -y build-essential libssl-dev
```

Then build:

```bash
g++ main.cpp -o notepad -lssl -lcrypto
```

Usage
-----

Save a note (type the note, finish with Ctrl+D):

```bash
./notepad save secret.note
```

You'll be prompted for a password (hidden). The file `secret.note` will contain a small header, the salt, and the ciphertext.

Read a note:

```bash
./notepad read secret.note
```

You'll be prompted for the password used to encrypt the note. If correct, the note will be printed.

Security notes
--------------

- This is a small example program. It uses PBKDF2 with 10,000 iterations (adjust as desired) and AES-256-CBC for encryption.
- Protect the encrypted file and choose a strong password.
- For production use, consider authenticated encryption (e.g., AES-GCM) and additional safeguards.

License
-------

Small example code — use as you wish.