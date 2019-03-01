cmake_minimum_required(VERSION 3.0)

project(WeeveClientApps)

link_directories("../export/lib")
link_directories("/home/gaurav/work/hiredis")
link_directories("/home/gaurav/work/libtomcrypt")
link_directories("/home/gaurav/work/libtommath")
include_directories("../export/include")

add_executable(simple_smp simple_smp.c)
target_link_libraries(simple_smp wcl tomcrypt tommath  hiredis )
