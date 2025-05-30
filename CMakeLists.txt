CU_SET_PATH("CMAKE_MOD_CUSTOMLOGIN_DIR" "${CMAKE_CURRENT_LIST_DIR}")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/mod_customlogin.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/loader_customlogin.cpp")
AC_ADD_SCRIPT_LOADER("mod_customlogin" "${CMAKE_CURRENT_LIST_DIR}/src/loader_customlogin.h")
