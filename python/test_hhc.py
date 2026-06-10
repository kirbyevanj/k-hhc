#!/usr/bin/env python3
"""
Unit tests for k-hhc Python bindings.
"""

import unittest
import k_hhc

GAP_CHARS = "/:;<=>?@[\\]^`{|}"


class TestHHCPython(unittest.TestCase):
    """Test cases for k-hhc Python bindings."""

    def test_32bit_encode_decode(self):
        """Test 32-bit encoding and decoding."""
        test_values = [0, 1, 42, 100, 1000, 424242, 4294967295]

        for value in test_values:
            padded = k_hhc.encode_padded_32bit(value)
            self.assertEqual(len(padded), k_hhc.HHC_32BIT_ENCODED_LENGTH)
            self.assertEqual(k_hhc.decode_32bit(padded), value)

            unpadded = k_hhc.encode_unpadded_32bit(value)
            self.assertEqual(k_hhc.decode_32bit(unpadded), value)

    def test_64bit_encode_decode(self):
        """Test 64-bit encoding and decoding."""
        test_values = [0, 1, 100, 1000000, 9876543210, 18446744073709551615]

        for value in test_values:
            padded = k_hhc.encode_padded_64bit(value)
            self.assertEqual(len(padded), k_hhc.HHC_64BIT_ENCODED_LENGTH)
            self.assertEqual(k_hhc.decode_64bit(padded), value)

            unpadded = k_hhc.encode_unpadded_64bit(value)
            self.assertEqual(k_hhc.decode_64bit(unpadded), value)

    def test_unpadded_zero_encodes_as_dash(self):
        """encode_unpadded(0) returns '-' and round-trips."""
        self.assertEqual(k_hhc.encode_unpadded_32bit(0), "-")
        self.assertEqual(k_hhc.encode_unpadded_64bit(0), "-")
        self.assertEqual(k_hhc.decode_32bit("-"), 0)
        self.assertEqual(k_hhc.decode_64bit("-"), 0)

    def test_gap_characters_raise_value_error(self):
        """Gap characters in ASCII range must not decode silently."""
        for ch in GAP_CHARS:
            with self.subTest(ch=ch):
                with self.assertRaises(ValueError):
                    k_hhc.decode_32bit("----" + ch)
                with self.assertRaises(ValueError):
                    k_hhc.decode_64bit("----" + ch)

        with self.assertRaises(ValueError):
            k_hhc.decode_32bit("//////")
        with self.assertRaises(ValueError):
            k_hhc.decode_64bit("//////")
        with self.assertRaises(ValueError):
            k_hhc.decode_32bit("-----:")
        with self.assertRaises(ValueError):
            k_hhc.decode_64bit("-----:")

    def test_decode_empty_string_raises_value_error(self):
        """Empty encoded strings are invalid."""
        with self.assertRaises(ValueError):
            k_hhc.decode_32bit("")
        with self.assertRaises(ValueError):
            k_hhc.decode_64bit("")

    def test_decode_non_str_raises_type_error(self):
        """decode requires str input."""
        for bad in (123, b"abc", None):
            with self.subTest(bad=bad):
                with self.assertRaises(TypeError):
                    k_hhc.decode_32bit(bad)
                with self.assertRaises(TypeError):
                    k_hhc.decode_64bit(bad)

    def test_encode_overflow(self):
        """Values outside unsigned range raise OverflowError."""
        with self.assertRaises(OverflowError):
            k_hhc.encode_padded_32bit(2**32)
        with self.assertRaises(OverflowError):
            k_hhc.encode_unpadded_32bit(2**32)
        with self.assertRaises(OverflowError):
            k_hhc.encode_padded_64bit(2**64)
        with self.assertRaises(OverflowError):
            k_hhc.encode_unpadded_64bit(2**64)

        for func in (
            k_hhc.encode_padded_32bit,
            k_hhc.encode_unpadded_32bit,
            k_hhc.encode_padded_64bit,
            k_hhc.encode_unpadded_64bit,
        ):
            with self.subTest(func=func.__name__):
                with self.assertRaises(OverflowError):
                    func(-1)

    def test_decode_errors(self):
        """Test decode error handling."""
        with self.assertRaises(ValueError):
            k_hhc.decode_32bit("INVALID!")

        with self.assertRaises(ValueError):
            k_hhc.decode_64bit("INVALID!")

        with self.assertRaises(OverflowError):
            k_hhc.decode_32bit("1QLCp2")  # UINT32_MAX + 1

        with self.assertRaises(OverflowError):
            k_hhc.decode_64bit("9lH9ebONzYE")  # UINT64_MAX + 1

    def test_boundary_round_trips_32bit(self):
        """Boundary values round-trip for 32-bit padded and unpadded."""
        for value in (0, 1, 65, 66, 2**32 - 1):
            with self.subTest(value=value):
                padded = k_hhc.encode_padded_32bit(value)
                self.assertEqual(k_hhc.decode_32bit(padded), value)
                unpadded = k_hhc.encode_unpadded_32bit(value)
                self.assertEqual(k_hhc.decode_32bit(unpadded), value)

    def test_boundary_round_trips_64bit(self):
        """Boundary values round-trip for 64-bit padded and unpadded."""
        for value in (0, 1, 2**32, 2**64 - 1):
            with self.subTest(value=value):
                padded = k_hhc.encode_padded_64bit(value)
                self.assertEqual(k_hhc.decode_64bit(padded), value)
                unpadded = k_hhc.encode_unpadded_64bit(value)
                self.assertEqual(k_hhc.decode_64bit(unpadded), value)

    def test_constants(self):
        """Test module constants."""
        self.assertEqual(k_hhc.HHC_32BIT_ENCODED_LENGTH, 6)
        self.assertEqual(k_hhc.HHC_64BIT_ENCODED_LENGTH, 11)
        self.assertEqual(len(k_hhc.ALPHABET), 66)
        self.assertTrue(k_hhc.ALPHABET.startswith("-.0123456789"))

    def test_edge_cases(self):
        """Test edge cases."""
        self.assertEqual(k_hhc.decode_32bit("-"), 0)
        self.assertEqual(k_hhc.decode_64bit("-"), 0)

        self.assertEqual(k_hhc.decode_32bit("."), 1)
        self.assertEqual(k_hhc.decode_64bit("."), 1)


if __name__ == "__main__":
    unittest.main()
