// To get rid of the _POSIX_C_SOURCE warning, make sure to include Python.h
// before all other header files.
#include <Python.h>

#include <iostream>

PyObject* convert_to_python(uint64_t v) {
    return PyLong_FromUnsignedLong(static_cast<unsigned long>(v));
}
PyObject* convert_to_python(int v) {
    return PyLong_FromLong(v);
}
PyObject* convert_to_python(const std::string& v) {
    return PyUnicode_FromString(v.c_str());
};
PyObject* convert_to_python(const char* v) {
    return PyUnicode_FromString(v);
};
PyObject* convert_to_python(double v) {
    return PyFloat_FromDouble(v);
};

PyObject* prepare_python() {

    // https://docs.python.org/3/c-api/init_config.html#c.PyConfig.use_environment
    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    // Set program name
    const wchar_t* program_name = L"pybrain";
    status = PyConfig_SetString(&config, &config.program_name, program_name);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        std::cerr << "Failed to set program name" << std::endl;
        return nullptr;
    }

    // Set custom Python path
    const wchar_t* python_path = L"."; // read Python scripts from current folder
    status = PyConfig_SetString(&config, &config.pythonpath_env, python_path);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        std::cerr << "Failed to set Python path" << std::endl;
        return nullptr;
    }

    // Set Python home
    const wchar_t* python_home = L"C:/msys64/mingw64/"; // it will add "lib/python3.12" automatically
    status = PyConfig_SetString(&config, &config.home, python_home);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        std::cerr << "Failed to set Python home" << std::endl;
        return nullptr;
    }

    // Initialize Python interpreter
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        std::cerr << "Failed to initialize Python" << std::endl;
        return nullptr;
    }

    PyConfig_Clear(&config); // Free config resources
    
    PyRun_SimpleString("import sys; print(sys.path)");
    PyRun_SimpleString("import sys; print(sys.prefix, sys.exec_prefix)");

    PyObject* pyModuleName = convert_to_python("neuron");
    PyObject* pyModule = PyImport_Import(pyModuleName);
    Py_DECREF(pyModuleName);
    
    if (!pyModule) {
        PyErr_Print();
        std::cerr << "Failed to load 'neuron'" << std::endl;
        return nullptr;
    }
    // load_value_python(pyModule, "prepare", convert_to_python(get_name()));
    return pyModule;
}

int main(int argc, char const *argv[])
{
    std::cout << "Embedded Python version: " << Py_GetVersion() << std::endl;

    PyObject* module = prepare_python();
    if (!module) {
        std::cerr << "Python module failed to prepare." << std::endl;
        return 1;
    }
    std::cout << "Python module prepared successfully!" << std::endl;

    Py_DECREF(module);
    Py_Finalize();

    return 0;
}
