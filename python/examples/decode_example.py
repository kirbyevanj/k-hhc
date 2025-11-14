#!/usr/bin/env python3
"""
Example demonstrating k-hhc decoding functionality in Python.
"""

import k_hhc

def decode_with_error_handling(decode_func, encoded, bit_size):
    """Helper function to decode with error handling."""
    try:
        decoded = decode_func(encoded)
        return f"{decoded}"
    except ValueError as e:
        return f"ValueError: {e}"
    except OverflowError as e:
        return f"OverflowError: {e}"
    except Exception as e:
        return f"Error: {e}"

def main():
    print("k-hhc Python Decoding Example")
    print("==============================\n")

    # Example 1: Decode 32-bit strings
    print("32-bit Decoding:")
    print("----------------")
    
    encoded_32 = [
        "------",      # 0 (padded)
        "-",           # 0 (unpadded)
        ".",           # 1 (unpadded)
        "-----A",      # 66 (padded)
        "1QLCp1",      # UINT32_MAX (padded)
        "--.TNv",      # 424242 (padded)
        ".TNv",        # 424242 (unpadded)
        "INVALID!",    # Invalid characters
        "1QLCp2",      # Exceeds 32-bit bounds
    ]
    
    for encoded in encoded_32:
        result = decode_with_error_handling(k_hhc.decode_32bit, encoded, 32)
        print(f"  '{encoded:10}' -> {result}")
    
    print()

    # Example 2: Decode 64-bit strings
    print("64-bit Decoding:")
    print("----------------")
    
    encoded_64 = [
        "-----------",        # 0 (padded)
        "-",                  # 0 (unpadded)
        ".",                  # 1 (unpadded)
        "9lH9ebONzYD",        # UINT64_MAX (padded)
        "-----5tVfK4",        # 9876543210 (padded)
        "5tVfK4",             # 9876543210 (unpadded)
        "INVALID!",           # Invalid characters
        "9lH9ebONzYE",        # Exceeds 64-bit bounds
    ]
    
    for encoded in encoded_64:
        result = decode_with_error_handling(k_hhc.decode_64bit, encoded, 64)
        print(f"  '{encoded:15}' -> {result}")
    
    print()

    # Example 3: Round-trip encoding and decoding
    print("Round-trip Example (32-bit):")
    print("----------------------------")
    
    test_values = [0, 1, 42, 100, 1000, 424242]
    
    for original in test_values:
        # Test with unpadded encoding
        encoded = k_hhc.encode_unpadded_32bit(original)
        # Handle empty string case for 0
        decode_str = "-" if encoded == "" else encoded
        decoded = k_hhc.decode_32bit(decode_str)
        status = "OK" if original == decoded else "FAIL"
        print(f"  {original:6} -> '{encoded:6}' -> {decoded:6} {status}")
    
    print()

    # Example 4: Performance test
    print("Performance Test (encoding/decoding 1M values):")
    print("-----------------------------------------------")
    
    import time
    
    # 32-bit test
    start = time.time()
    for i in range(1_000_000):
        encoded = k_hhc.encode_unpadded_32bit(i)
        if encoded:  # Skip empty string for 0
            decoded = k_hhc.decode_32bit(encoded)
    elapsed = time.time() - start
    print(f"  32-bit: 1M encode/decode pairs in {elapsed:.3f} seconds")
    print(f"          ({1_000_000/elapsed:,.0f} operations/second)")

if __name__ == "__main__":
    main()
