#!/usr/bin/env python3
"""
Example demonstrating k-hhc encoding functionality in Python.
"""

import k_hhc

def main():
    print("k-hhc Python Encoding Example")
    print("==============================\n")

    # Example 1: Encode 32-bit values
    print("32-bit Encoding:")
    print("----------------")
    
    values_32 = [0, 1, 42, 1000, 424242, 4294967295]
    
    for value in values_32:
        padded = k_hhc.encode_padded_32bit(value)
        unpadded = k_hhc.encode_unpadded_32bit(value)
        print(f"  {value:10} -> '{padded}' (padded), '{unpadded}' (unpadded)")
    
    print()

    # Example 2: Encode 64-bit values
    print("64-bit Encoding:")
    print("----------------")
    
    values_64 = [0, 1, 100, 1000000, 9876543210, 18446744073709551615]
    
    for value in values_64:
        padded = k_hhc.encode_padded_64bit(value)
        unpadded = k_hhc.encode_unpadded_64bit(value)
        print(f"  {value:20} -> '{padded}' (padded), '{unpadded}' (unpadded)")
    
    print()

    # Example 3: Show alphabet
    print(f"k-hhc Alphabet: '{k_hhc.ALPHABET}'")
    print(f"32-bit encoded length: {k_hhc.HHC_32BIT_ENCODED_LENGTH} characters")
    print(f"64-bit encoded length: {k_hhc.HHC_64BIT_ENCODED_LENGTH} characters")

if __name__ == "__main__":
    main()
