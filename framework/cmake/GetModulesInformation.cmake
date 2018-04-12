function(get_modules_informatrion module)
    string(TOUPPER ${module} module_name_upper)
    string(TOLOWER ${module} module_name_lower)
    set(${module_name_upper}_INCLUDE_PATH
        ${PROJECT_SOURCE_DIR}/${module_name_lower}/include PARENT_SCOPE)
    include_directories(${PROJECT_SOURCE_DIR}/${module_name_lower}/include)

    set(${module_name_upper}_SRC_PATH
        ${PROJECT_SOURCE_DIR}/${module_name_lower}/src PARENT_SCOPE)
    file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/${module_name_lower}/*.cpp)
    set(${module_name_upper}_SRC_LIST ${src_files} PARENT_SCOPE)
endfunction()
