# Codernix ED25519 PHP Extension

A high-performance PHP extension for the Ed25519 digital signature system, built on top of a custom, highly optimized assembly implementation from the SUPERCOP crypto bundle. The original assembly code was adapted to be fully position-independent (PIC), a necessary step to create a stable and portable shared library for PHP.


This extension provides fast and secure functions for key generation, signing messages, and verifying signatures, including support for batch verification.

## Features

* **High Speed**: Leverages an optimized `amd64-64-24k` assembly implementation for maximum performance on x86-64 servers.
* **Secure**: Implements the modern and widely trusted Ed25519 signature algorithm.
* **Batch Verification**: Includes `ed25519_open_batch()` for efficiently verifying a large number of signatures at once.
* **Simple API**: Provides a clear and straightforward set of functions for use in any PHP application.

## Installation

This extension is designed to be compiled against a pre-built shared library (`libcrypto_sign_ed25519.so`).

1. **Build the Dependency**: First, build the `libcrypto_sign_ed25519.so` shared library and its headers using the `supercop-builder` service from our build system.
2. **Compile the Extension**:
   ```bash
   # Clone the repository
   git clone [https://github.com/codernix/ed25519-php-ext.git](https://github.com/codernix/ed25519-php-ext.git)
   cd ed25519-php-ext
   
   # Prepare for compilation
   phpize
   
   # Configure, pointing to the location of the library and headers
   LDFLAGS="-L/path/to/lib" CPPFLAGS="-I/path/to/include" ./configure
   
   # Compile
   make
   ```
3. **Enable the Extension**: Copy the resulting `ed25519.so` file to your PHP extension directory and add `extension=ed25519.so` to your `php.ini` file.

## API and Usage

### `ed25519_keypair(): array`

Generates a new key pair.

**Returns**: An associative array `['public_key' => string, 'secret_key' => string]`.

```php
$keys = ed25519_keypair();
$publicKey = $keys['public_key']; // 32 bytes
$secretKey = $keys['secret_key']; // 64 bytes
```

### `ed25519_sign(string $message, string $secret_key): string`

Signs a message using a secret key.

**Returns**: The signed message (64-byte signature + original message).

```php
$message = "This is a test message.";
$signedMessage = ed25519_sign($message, $secretKey);
```

### `ed25519_open(string $signed_message, string $public_key): string|false`

Verifies a signed message and returns the original message if the signature is valid.

**Returns**: The original message on success, or `false` on failure.

```php
$originalMessage = ed25519_open($signedMessage, $publicKey);

if ($originalMessage !== false) {
    echo "Signature is valid! Message: " . $originalMessage;
} else {
    echo "Signature is invalid!";
}
```

### `ed25519_open_batch(array $signed_messages, array $public_keys): array`

Verifies a batch of signed messages. This is significantly faster than calling `ed25519_open()` in a loop.

**Returns**: An array where each element is either the original message (on success) or `false` (on failure).

```php
// Generate some data for the batch
$batchSize = 5;
$signedMessages = [];
$publicKeys = [];

for ($i = 0; $i < $batchSize; $i++) {
    $keys = ed25519_keypair();
    $publicKeys[] = $keys['public_key'];
    $signedMessages[] = ed25519_sign("Message #$i", $keys['secret_key']);
}

// Tamper with one signature to test failure
$signedMessages[2] = substr_replace($signedMessages[2], 'X', 3, 1);

// Verify the batch
$results = ed25519_open_batch($signedMessages, $publicKeys);

foreach ($results as $index => $message) {
    if ($message !== false) {
        printf("Message %d is valid: %s\n", $index, $message);
    } else {
        printf("Message %d is INVALID\n", $index);
    }
}
```

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
