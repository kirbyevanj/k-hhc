// Build note:
// - Compile as C++ with the CPython Limited API (abi3).
// - Define Py_LIMITED_API to the minimum Python 3 version you want to support (e.g., 0x03090000 for 3.9+).
//
// Limited API (PEP 384) extension to produce abi3-compatible wheels.

#include <Python.h>

#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <limits>

#include "hhc.hpp"

using std::snprintf;
using std::strlen;
using std::numeric_limits;
using std::invalid_argument;
using std::out_of_range;
using std::exception;

using hhc::hhc_32bit_encode_padded;
using hhc::hhc_32bit_encode_unpadded;
using hhc::hhc_32bit_decode;
using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;
using hhc::hhc_64bit_decode;

using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;
using hhc::ALPHABET;

/**
 * RAII helper to decref a PyObject* on scope exit.
*/
struct PyObjHolder {
    PyObject* obj;
    explicit PyObjHolder(PyObject* o) : obj(o) {}
    ~PyObjHolder() { Py_XDECREF(obj); }
    PyObject* get() const { return obj; }
    PyObject* release() { PyObject* tmp = obj; obj = nullptr; return tmp; }
};

/**
 * Set a Python overflow error for a given function and bit size.
 * @param func The function name.
 * @param bits The bit size.
 */
static void set_overflow_for_bits(const char* func, const char* bits) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s: value out of range for %s unsigned integer", func, bits);
    PyErr_SetString(PyExc_OverflowError, msg);
}

/**
 * Translate a C++ exception to a Python exception.
 * @note This function rethrows the current exception, only run this in a catch block.
 */
static void translate_std_exception() {
    try {
        throw;
    } catch (const invalid_argument& e) {
        PyErr_SetString(PyExc_ValueError, e.what());
    } catch (const out_of_range& e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
    } catch (const exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
    }
}

/**
 * Encode a 32-bit unsigned integer to a padded 6-character string.
 * @param self The Python object.
 * @param arg The value to encode.
 * @return The encoded string.
 */
static PyObject* k_hhc_encode_padded_32bit(PyObject*, PyObject* arg) {
    uint64_t v = PyLong_AsUnsignedLongLong(arg);
    if (v == numeric_limits<uint32_t>::max() && PyErr_Occurred()) {
        return nullptr;
    }
    if (v > 0xFFFFFFFFULL) {
        set_overflow_for_bits("encode_padded_32bit", "32-bit");
        return nullptr;
    }

    try {
        char result[hhc::HHC_32BIT_STRING_LENGTH] = {};
        hhc_32bit_encode_padded(static_cast<uint32_t>(v), result);
        return PyUnicode_FromStringAndSize(result, HHC_32BIT_ENCODED_LENGTH);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Encode a 32-bit unsigned integer to an unpadded string.
 * @param self The Python object.
 * @param arg The value to encode.
 * @return The encoded string.
 */
static PyObject* k_hhc_encode_unpadded_32bit(PyObject*, PyObject* arg) {
    uint64_t v = PyLong_AsUnsignedLongLong(arg);
    if (v == numeric_limits<uint32_t>::max() && PyErr_Occurred()) {
        return nullptr;
    }
    if (v > 0xFFFFFFFFULL) {
        set_overflow_for_bits("encode_unpadded_32bit", "32-bit");
        return nullptr;
    }

    try {
        char result[HHC_32BIT_STRING_LENGTH] = {};
        hhc_32bit_encode_unpadded(static_cast<uint32_t>(v), result);
        const size_t len = strlen(result);
        return PyUnicode_FromStringAndSize(result, (Py_ssize_t)len);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Decode a 32-bit unsigned integer from a string.
 * @param self The Python object.
 * @param arg The encoded string.
 * @return The decoded integer.
 */
static PyObject* k_hhc_decode_32bit(PyObject*, PyObject* arg) {
    if (!PyUnicode_Check(arg)) {
        PyErr_SetString(PyExc_TypeError, "encoded must be a str");
        return nullptr;
    }

    PyObject* bytes = PyUnicode_AsUTF8String(arg); // new reference
    if (!bytes) {
        return nullptr;
    }
    PyObjHolder bytes_holder(bytes);

    char* s = nullptr;
    if (PyBytes_AsStringAndSize(bytes_holder.get(), &s, nullptr) < 0) {
        return nullptr;
    }

    try {
        uint32_t decoded = hhc_32bit_decode(s);
        return PyLong_FromUnsignedLong((unsigned long)decoded);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Encode a 64-bit unsigned integer to a padded 11-character string.
 * @param self The Python object.
 * @param arg The value to encode.
 * @return The encoded string.
 */
static PyObject* k_hhc_encode_padded_64bit(PyObject*, PyObject* arg) {
    uint64_t v = PyLong_AsUnsignedLongLong(arg);
    if (v == numeric_limits<uint64_t>::max() && PyErr_Occurred()) {
        return nullptr;
    }

    try {
        char result[HHC_64BIT_STRING_LENGTH] = {};
        hhc_64bit_encode_padded(static_cast<uint64_t>(v), result);
        return PyUnicode_FromStringAndSize(result, HHC_64BIT_ENCODED_LENGTH);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Encode a 64-bit unsigned integer to an unpadded string.
 * @param self The Python object.
 * @param arg The value to encode.
 * @return The encoded string.
 */
static PyObject* k_hhc_encode_unpadded_64bit(PyObject* /*self*/, PyObject* arg) {
    uint64_t v = PyLong_AsUnsignedLongLong(arg);

    try {
        char result[HHC_64BIT_STRING_LENGTH] = {};
        hhc_64bit_encode_unpadded(v, result);
        const size_t len = strlen(result);
        return PyUnicode_FromStringAndSize(result, (Py_ssize_t)len);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Decode a 64-bit unsigned integer from a string.
 * @param self The Python object.
 * @param arg The encoded string.
 * @return The decoded integer.
 */
static PyObject* k_hhc_decode_64bit(PyObject* /*self*/, PyObject* arg) {
    if (!PyUnicode_Check(arg)) {
        PyErr_SetString(PyExc_TypeError, "encoded must be a str");
        return nullptr;
    }

    PyObject* bytes = PyUnicode_AsUTF8String(arg); // new reference
    if (!bytes) {
        return nullptr;
    }
    PyObjHolder bytes_holder(bytes);

    char* s = nullptr;
    if (PyBytes_AsStringAndSize(bytes_holder.get(), &s, nullptr) < 0) {
        return nullptr;
    }

    try {
        uint64_t decoded = hhc_64bit_decode(s);
        return PyLong_FromUnsignedLongLong((unsigned long long)decoded);
    } catch (...) {
        translate_std_exception();
        return nullptr;
    }
}

/**
 * Module documentation.
 * @return The module documentation.
 */
static const char* k_hhc_module_doc =
    "k-hhc (Hexahexacontadecimal) Python bindings (abi3 limited API)\n\n"
    "Exports:\n"
    "- encode_padded_32bit(value: int) -> str\n"
    "- encode_unpadded_32bit(value: int) -> str\n"
    "- decode_32bit(encoded: str) -> int\n"
    "- encode_padded_64bit(value: int) -> str\n"
    "- encode_unpadded_64bit(value: int) -> str\n"
    "- decode_64bit(encoded: str) -> int\n";

/**
 * Documentation for encode_padded_32bit.
 * @return The documentation.
 */
static const char* doc_encode_padded_32bit =
    "Encode a 32-bit unsigned integer to a padded 6-character string.\n\n"
    "Args:\n"
    "    value (int): The 32-bit unsigned integer to encode (0 to 4294967295).\n\n"
    "Returns:\n"
    "    str: A 6-character string with padding.";

/**
 * Documentation for encode_unpadded_32bit.
 * @return The documentation.
 */
static const char* doc_encode_unpadded_32bit =
    "Encode a 32-bit unsigned integer to an unpadded string.\n\n"
    "Args:\n"
    "    value (int): The 32-bit unsigned integer to encode (0 to 4294967295).\n\n"
    "Returns:\n"
    "    str: A variable-length string without padding (empty string for 0).";

/**
 * Documentation for decode_32bit.
 * @return The documentation.
 */
static const char* doc_decode_32bit =
    "Decode a string to a 32-bit unsigned integer.\n\n"
    "Args:\n"
    "    encoded (str): The encoded string (padded or unpadded).\n\n"
    "Returns:\n"
    "    int: The decoded 32-bit unsigned integer.\n\n"
    "Raises:\n"
    "    ValueError: If the string contains invalid characters.\n"
    "    OverflowError: If the string represents a value exceeding 32-bit bounds.";

/**
 * Documentation for encode_padded_64bit.
 * @return The documentation.
 */
static const char* doc_encode_padded_64bit =
    "Encode a 64-bit unsigned integer to a padded 11-character string.\n\n"
    "Args:\n"
    "    value (int): The 64-bit unsigned integer to encode (0 to 18446744073709551615).\n\n"
    "Returns:\n"
    "    str: An 11-character string with padding.";

/**
 * Documentation for encode_unpadded_64bit.
 * @return The documentation.
 */
static const char* doc_encode_unpadded_64bit =
    "Encode a 64-bit unsigned integer to an unpadded string.\n\n"
    "Args:\n"
    "    value (int): The 64-bit unsigned integer to encode (0 to 18446744073709551615).\n\n"
    "Returns:\n"
    "    str: A variable-length string without padding (empty string for 0).";

/**
 * Documentation for decode_64bit.
 * @return The documentation.
 */
static const char* doc_decode_64bit =
    "Decode a string to a 64-bit unsigned integer.\n\n"
    "Args:\n"
    "    encoded (str): The encoded string (padded or unpadded).\n\n"
    "Returns:\n"
    "    int: The decoded 64-bit unsigned integer.\n\n"
    "Raises:\n"
    "    ValueError: If the string contains invalid characters.\n"
    "    OverflowError: If the string represents a value exceeding 64-bit bounds.";

/**
 * Method table.
 * @return The method table PyMethodDef.
 */
static PyMethodDef k_hhc_methods[] = {
    {"encode_padded_32bit",   (PyCFunction)k_hhc_encode_padded_32bit,   METH_O, doc_encode_padded_32bit},
    {"encode_unpadded_32bit", (PyCFunction)k_hhc_encode_unpadded_32bit, METH_O, doc_encode_unpadded_32bit},
    {"decode_32bit",          (PyCFunction)k_hhc_decode_32bit,          METH_O, doc_decode_32bit},
    {"encode_padded_64bit",   (PyCFunction)k_hhc_encode_padded_64bit,   METH_O, doc_encode_padded_64bit},
    {"encode_unpadded_64bit", (PyCFunction)k_hhc_encode_unpadded_64bit, METH_O, doc_encode_unpadded_64bit},
    {"decode_64bit",          (PyCFunction)k_hhc_decode_64bit,          METH_O, doc_decode_64bit},
    {nullptr, nullptr, 0, nullptr}
};

/**
 * Module definition.
 * @return The module definition PyModuleDef.
 */
static struct PyModuleDef k_hhc_module = {
    .m_base     = PyModuleDef_HEAD_INIT,
    .m_name     = "k_hhc",
    .m_doc      = k_hhc_module_doc,
    .m_size     = -1,
    .m_methods  = k_hhc_methods,
    .m_slots    = nullptr,
    .m_traverse = nullptr,
    .m_clear    = nullptr,
    .m_free     = nullptr
};

/**
 * Module initialization.
 * @return The module initialization PyMODINIT_FUNC.
 */
PyMODINIT_FUNC PyInit_k_hhc(void) {
    PyObject* m = PyModule_Create(&k_hhc_module);
    if (!m) {
        return nullptr;
    }

    // Module-level integer constants
    {
        PyObject* v = PyLong_FromUnsignedLongLong((unsigned long long)hhc::HHC_32BIT_ENCODED_LENGTH);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_32BIT_ENCODED_LENGTH", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }
    {
        PyObject* v = PyLong_FromUnsignedLongLong((unsigned long long)hhc::HHC_64BIT_ENCODED_LENGTH);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_64BIT_ENCODED_LENGTH", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }
    {
        PyObject* v = PyLong_FromUnsignedLongLong((unsigned long long)hhc::HHC_32BIT_STRING_LENGTH);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_32BIT_STRING_LENGTH", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }
    {
        PyObject* v = PyLong_FromUnsignedLongLong((unsigned long long)hhc::HHC_64BIT_STRING_LENGTH);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_64BIT_STRING_LENGTH", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }

    // Module-level string constants
    {
        PyObject* v = PyUnicode_FromString(hhc::HHC_32BIT_ENCODED_MAX_STRING);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_32BIT_ENCODED_MAX_STRING", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }
    {
        PyObject* v = PyUnicode_FromString(hhc::HHC_64BIT_ENCODED_MAX_STRING);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "HHC_64BIT_ENCODED_MAX_STRING", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }

    // ALPHABET: convert container to Python str
    {
        constexpr size_t alphabet_size = hhc::ALPHABET.size();
        char alphabet_str[alphabet_size + 1] = {};
        for (size_t i = 0; i < alphabet_size; ++i) {
            alphabet_str[i] = hhc::ALPHABET[i];
        }
        PyObject* v = PyUnicode_FromStringAndSize(alphabet_str, (Py_ssize_t)alphabet_size);
        if (!v) { Py_DECREF(m); return nullptr; }
        if (PyModule_AddObject(m, "ALPHABET", v) < 0) { Py_DECREF(v); Py_DECREF(m); return nullptr; }
    }

    return m;
}