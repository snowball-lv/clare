
require_relative "rakefile-base"
require "pathname"

# output names
NAME = "clare"
DYN_LIB = "bin/#{NAME}.dll"

# build files to use as extra file dependencies
BUILD_FILES = [
    "rakefile",
    "rakefile-base.rb",
    "rakefile-main.rb",
    "rakefile-gen.rb" ]

# enumerate compiler source and object files
SRCS = FileList["src/**/*.c"]
OBJS = SRCS.pathmap("out/%X.o")

# different compile flags
CFLAGS = [
    "-c",
    "-I inc",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Werror"
]
DYN_CFLAGS = CFLAGS + [ "-fPIC" ]

# create compile tasks for the compiler source
SRCS.zip(OBJS).each do |src, obj|
    make_compile_task(src, obj, DYN_CFLAGS, BUILD_FILES)
end

# different link flags
LFLAGS = []
DYN_LFLAGS = LFLAGS + [ "-shared" ]
DEV_LFLAGS = LFLAGS + [ "-L bin", "-l #{NAME}" ]

# create linking task
make_link_task(DYN_LIB, OBJS, DYN_LFLAGS, BUILD_FILES)

# enumerate source and object files for development tests
DEV_SRCS = FileList["dev/**/*.c"]
DEV_OBJS = DEV_SRCS.pathmap("out/%X.o")
DEV_BINS = DEV_SRCS.pathmap("%X").map do |path|
    name = Pathname(path).each_filename.to_a.join("-")
    File.join("bin", name)
end

# create compile tasks for development test source files
DEV_SRCS.zip(DEV_OBJS).each do |src, obj|
    make_compile_task(src, obj, CFLAGS, BUILD_FILES)
end

# create link tasks for development tests
DEV_BINS.zip(DEV_OBJS).each do |bin, obj|
    file(bin => DYN_LIB)
    make_link_task(bin, [ obj ], DEV_LFLAGS, BUILD_FILES)
end

# task :default => DYN_LIB
task :default => DEV_BINS do
    run_tests(DEV_BINS)
end

task :clean do
    rm_rf "out"
    rm_rf "bin"
    rm_rf Dir["*.exe.stackdump"]
end
