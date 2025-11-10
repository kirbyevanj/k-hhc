#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "hhc.hpp"
#include <string>
#include <stdexcept>

namespace py = pybind11;

/**
 * @file hhc_python.cpp
 * @brief Python bindings for the HHC library using pybind11.
 * 
 * Exports six functions:
 * - encode_padded_32bit: Encode a 32-bit integer to a padded 6-character string
 * - encode_unpadded_32bit: Encode a 32-bit integer to an unpadded string (variable length)
 * - decode_32bit: Decode a string to a 32-bit integer
 * - encode_padded_64bit: Encode a 64-bit integer to a padded 11-character string
 * - encode_unpadded_64bit: Encode a 64-bit integer to an unpadded string (variable length)
 * - decode_64bit: Decode a string to a 64-bit integer
 */

std::string encode_padded_32bit(uint32_t value) {
    std::string result(hhc::HHC_32BIT_ENCODED_LENGTH, '\0');
    hhc::hhc_32bit_encode_padded(value, &result[0]);
    return result;
}

std::string encode_unpadded_32bit(uint32_t value) {
    std::string result(hhc::HHC_32BIT_STRING_LENGTH, '\0');
    hhc::hhc_32bit_encode_unpadded(value, &result[0]);
    // Find the actual length (until null terminator)
    return std::string(result.c_str());
}

uint32_t decode_32bit(const std::string& encoded) {
    return hhc::hhc_32bit_decode(encoded.c_str());
}

std::string encode_padded_64bit(uint64_t value) {
    std::string result(hhc::HHC_64BIT_ENCODED_LENGTH, '\0');
    hhc::hhc_64bit_encode_padded(value, &result[0]);
    return result;
}

std::string encode_unpadded_64bit(uint64_t value) {
    std::string result(hhc::HHC_64BIT_STRING_LENGTH, '\0');
    hhc::hhc_64bit_encode_unpadded(value, &result[0]);
    // Find the actual length (until null terminator)
    return std::string(result.c_str());
}

uint64_t decode_64bit(const std::string& encoded) {
    return hhc::hhc_64bit_decode(encoded.c_str());
}

PYBIND11_MODULE(k_hhc, m) {
    m.doc() = "k-hhc (Hexahexacontadecimal) Python bindings";

    // 32-bit functions
    m.def("encode_padded_32bit", &encode_padded_32bit, py::arg("value"),
          "Encode a 32-bit unsigned integer to a padded 6-character string.\n\n"
          "Args:\n"
          "    value (int): The 32-bit unsigned integer to encode (0 to 4294967295).\n\n"
          "Returns:\n"
          "    str: A 6-character string with padding.");

    m.def("encode_unpadded_32bit", &encode_unpadded_32bit, py::arg("value"),
          "Encode a 32-bit unsigned integer to an unpadded string.\n\n"
          "Args:\n"
          "    value (int): The 32-bit unsigned integer to encode (0 to 4294967295).\n\n"
          "Returns:\n"
          "    str: A variable-length string without padding (empty string for 0).");

    m.def("decode_32bit", &decode_32bit, py::arg("encoded"),
          "Decode a string to a 32-bit unsigned integer.\n\n"
          "Args:\n"
          "    encoded (str): The encoded string (padded or unpadded).\n\n"
          "Returns:\n"
          "    int: The decoded 32-bit unsigned integer.\n\n"
          "Raises:\n"
          "    ValueError: If the string contains invalid characters.\n"
          "    OverflowError: If the string represents a value exceeding 32-bit bounds.");

    // 64-bit functions
    m.def("encode_padded_64bit", &encode_padded_64bit, py::arg("value"),
          "Encode a 64-bit unsigned integer to a padded 11-character string.\n\n"
          "Args:\n"
          "    value (int): The 64-bit unsigned integer to encode (0 to 18446744073709551615).\n\n"
          "Returns:\n"
          "    str: An 11-character string with padding.");

    m.def("encode_unpadded_64bit", &encode_unpadded_64bit, py::arg("value"),
          "Encode a 64-bit unsigned integer to an unpadded string.\n\n"
          "Args:\n"
          "    value (int): The 64-bit unsigned integer to encode (0 to 18446744073709551615).\n\n"
          "Returns:\n"
          "    str: A variable-length string without padding (empty string for 0).");

    m.def("decode_64bit", &decode_64bit, py::arg("encoded"),
          "Decode a string to a 64-bit unsigned integer.\n\n"
          "Args:\n"
          "    encoded (str): The encoded string (padded or unpadded).\n\n"
          "Returns:\n"
          "    int: The decoded 64-bit unsigned integer.\n\n"
          "Raises:\n"
          "    ValueError: If the string contains invalid characters.\n"
          "    OverflowError: If the string represents a value exceeding 64-bit bounds.");

    // Register exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_OverflowError, e.what());
        }
    });

    // Module-level constants
    m.attr("HHC_32BIT_ENCODED_LENGTH") = hhc::HHC_32BIT_ENCODED_LENGTH;
    m.attr("HHC_64BIT_ENCODED_LENGTH") = hhc::HHC_64BIT_ENCODED_LENGTH;
    m.attr("HHC_32BIT_STRING_LENGTH")  = hhc::HHC_32BIT_STRING_LENGTH;
    m.attr("HHC_64BIT_STRING_LENGTH")  = hhc::HHC_64BIT_STRING_LENGTH;
    m.attr("HHC_32BIT_ENCODED_MAX_STRING") = hhc::HHC_32BIT_ENCODED_MAX_STRING;
    m.attr("HHC_64BIT_ENCODED_MAX_STRING") = hhc::HHC_64BIT_ENCODED_MAX_STRING;
    
    // Convert ALPHABET array to string
    std::string alphabet_str(hhc::ALPHABET.begin(), hhc::ALPHABET.end());
    m.attr("ALPHABET") = alphabet_str;
}
