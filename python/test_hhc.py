#!/usr/bin/env python3
"""
Unit tests for k-hhc Python bindings.
"""

import unittest
import k_hhc


class TestHHCPython(unittest.TestCase):
    """Test cases for k-hhc Python bindings."""

    def test_32bit_encode_decode(self):
        """Test 32-bit encoding and decoding."""
        test_values = [0, 1, 42, 100, 1000, 424242, 4294967295]
        
        for value in test_values:
            # Test padded encoding
            padded = k_hhc.encode_padded_32bit(value)
            self.assertEqual(len(padded), k_hhc.HHC_32BIT_ENCODED_LENGTH)
            decoded = k_hhc.decode_32bit(padded)
            self.assertEqual(decoded, value)
            
            # Test unpadded encoding
            unpadded = k_hhc.encode_unpadded_32bit(value)
            # Handle empty string for 0
            decode_str = "-" if unpadded == "" else unpadded
            decoded = k_hhc.decode_32bit(decode_str)
            self.assertEqual(decoded, value)

    def test_64bit_encode_decode(self):
        """Test 64-bit encoding and decoding."""
        test_values = [0, 1, 100, 1000000, 9876543210, 18446744073709551615]
        
        for value in test_values:
            # Test padded encoding
            padded = k_hhc.encode_padded_64bit(value)
            self.assertEqual(len(padded), k_hhc.HHC_64BIT_ENCODED_LENGTH)
            decoded = k_hhc.decode_64bit(padded)
            self.assertEqual(decoded, value)
            
            # Test unpadded encoding
            unpadded = k_hhc.encode_unpadded_64bit(value)
            # Handle empty string for 0
            decode_str = "-" if unpadded == "" else unpadded
            decoded = k_hhc.decode_64bit(decode_str)
            self.assertEqual(decoded, value)

    def test_decode_errors(self):
        """Test decode error handling."""
        # Invalid characters
        with self.assertRaises(ValueError):
            k_hhc.decode_32bit("INVALID!")
        
        with self.assertRaises(ValueError):
            k_hhc.decode_64bit("INVALID!")
        
        # Exceeds bounds
        with self.assertRaises(OverflowError):
            k_hhc.decode_32bit("1QLCp2")  # UINT32_MAX + 1
        
        with self.assertRaises(OverflowError):
            k_hhc.decode_64bit("9lH9ebONzYE")  # UINT64_MAX + 1

    def test_constants(self):
        """Test module constants."""
        self.assertEqual(k_hhc.HHC_32BIT_ENCODED_LENGTH, 6)
        self.assertEqual(k_hhc.HHC_64BIT_ENCODED_LENGTH, 11)
        self.assertEqual(len(k_hhc.ALPHABET), 66)
        self.assertTrue(k_hhc.ALPHABET.startswith("-.0123456789"))

    def test_edge_cases(self):
        """Test edge cases."""
        # Empty string handling for decode
        self.assertEqual(k_hhc.decode_32bit("-"), 0)
        self.assertEqual(k_hhc.decode_64bit("-"), 0)
        
        # Single character decoding
        self.assertEqual(k_hhc.decode_32bit("."), 1)
        self.assertEqual(k_hhc.decode_64bit("."), 1)
        
        # Unpadded encoding of 0 produces empty string
        self.assertEqual(k_hhc.encode_unpadded_32bit(0), "")
        self.assertEqual(k_hhc.encode_unpadded_64bit(0), "")


if __name__ == "__main__":
    unittest.main()
