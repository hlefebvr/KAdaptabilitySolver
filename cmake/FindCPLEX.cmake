if (NOT CPLEX_INSTALL_PATH OR CPLEX_INSTALL_PATH STREQUAL "")
    set(DEFAULT_CPLEX_INSTALL_PATH "/opt/ibm/ILOG/CPLEX_Studio*")
    message(STATUS "Parameter CPLEX_INSTALL_PATH was not given, looking for IBM CPLEX inside ${DEFAULT_CPLEX_INSTALL_PATH}")

    file(GLOB CPLEX_DIRS ${DEFAULT_CPLEX_INSTALL_PATH})
    list(LENGTH CPLEX_DIRS CPLEX_DIRS_LENGTH)

    if (CPLEX_DIRS_LENGTH EQUAL 0)
        message(FATAL_ERROR "Looking for IBM CPLEX inside ${DEFAULT_CPLEX_INSTALL_PATH} failed.")
    endif()

    list(GET CPLEX_DIRS 0 CPLEX_INSTALL_PATH)
    message(STATUS "Set CPLEX_INSTALL_PATH=${CPLEX_INSTALL_PATH}")
else()
    message(STATUS "User provided CPLEX_INSTALL_PATH=${CPLEX_INSTALL_PATH}")
endif()

string(CONCAT CPLEX_DIR ${CPLEX_INSTALL_PATH};/cplex)
string(CONCAT CONCERT_DIR ${CPLEX_INSTALL_PATH};/concert)

find_path(CPLEX_INCLUDE_DIR ilocplex.h HINTS "${CPLEX_DIR}/include/ilcplex")
find_path(CONCERT_INCLUDE_DIR iloalg.h HINTS "${CONCERT_DIR}/include/ilconcert")

if(APPLE)
    find_library(CPLEX_CPP_LIBRARY libcplex.a HINTS "${CPLEX_DIR}/lib/x86-64_osx/static_pic")
    find_library(CONCERT_CPP_LIBRARY libconcert.a HINTS "${CONCERT_DIR}/lib/x86-64_osx/static_pic")
    find_library(CPLEX_ILO_CPP_LIBRARY libilocplex.a HINTS "${CPLEX_DIR}/lib/x86-64_osx/static_pic")
elseif(UNIX)
    find_library(CPLEX_CPP_LIBRARY libcplex.a x HINTS "${CPLEX_DIR}/lib/x86-64_linux/static_pic")
    find_library(CONCERT_CPP_LIBRARY libconcert.a libcplexdistmip.a HINTS "${CONCERT_DIR}/lib/x86-64_linux/static_pic")
    find_library(CPLEX_ILO_CPP_LIBRARY libilocplex.a HINTS "${CPLEX_DIR}/lib/x86-64_linux/static_pic")
else()
    message(FATAL_ERROR "ONLY UNIX IS SUPPORTED AT THIS TIME")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPLEX DEFAULT_MSG CPLEX_CPP_LIBRARY  CPLEX_ILO_CPP_LIBRARY CONCERT_CPP_LIBRARY  CPLEX_INCLUDE_DIR CONCERT_INCLUDE_DIR)

if(CPLEX_FOUND)
    # ilo_cpp
    add_library(solver::cplex::ilo SHARED IMPORTED)
    set_target_properties(solver::cplex::ilo PROPERTIES
            IMPORTED_LOCATION ${CPLEX_ILO_CPP_LIBRARY}
        )

    # cplex_cpp
    add_library(solver::cplex::cplex SHARED IMPORTED)
    set_target_properties(solver::cplex::cplex PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${CPLEX_INCLUDE_DIR}/..
            IMPORTED_LOCATION ${CPLEX_CPP_LIBRARY}
        )

    # concert_cpp
    add_library(solver::cplex::concert SHARED IMPORTED)
    set_target_properties(solver::cplex::concert PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${CONCERT_INCLUDE_DIR}/..
            IMPORTED_LOCATION ${CONCERT_CPP_LIBRARY}
        )

    add_library(solver::cplex INTERFACE IMPORTED)
    set_property(TARGET solver::cplex PROPERTY
            INTERFACE_LINK_LIBRARIES solver::cplex::ilo solver::cplex::cplex solver::cplex::concert m pthread dl)
else()
    message(FATAL_ERROR "Could not find solver IBM CPLEX")
endif(CPLEX_FOUND)
