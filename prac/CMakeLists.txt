cmake_minimum_required(VERSION 3.10)


add_subdirectory(img_pro)
add_subdirectory(he)
add_subdirectory(ahe)
add_subdirectory(clhe)
add_subdirectory(clahe)
add_subdirectory(clip)
if(UINX)
    add_subdirectory(sh_gl)
endif()