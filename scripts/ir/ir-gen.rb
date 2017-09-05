#!/usr/bin/env ruby

require "erb"
require "fileutils"

def render(template, output)
    renderer = ERB.new(File.read(template))
    File.write(output, renderer.result())
end


INC = "output/inc/ir"
SRC = "output/src/ir"

FileUtils.mkdir_p(INC)
FileUtils.mkdir_p(SRC)

render "templates/IR.h.erb", "#{INC}/IR.h"
render "templates/IR.c.erb", "#{SRC}/IR.c"
