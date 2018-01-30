#!/usr/bin/env ruby

require "fileutils"


Dir["dev/**/*.c"].each do |path|
    
    puts
    puts path
    text = File.read(path)
    
    File.open(path, "w") do |file|
        
        text.each_line do |line|
            
            m = line.strip.match(/^#include <(?<ipath>.+?)>$/)
            if m
                
                ipath = m["ipath"]
                fpath = File.join("inc", "clare", ipath)
                if File.file?(fpath)
                    npath = File.join("clare", ipath)
                    puts "#{ipath} -> #{npath}"
                    file.puts "\#include <#{npath}>"
                else
                    file.puts line
                end
                
            else
                file.puts line
            end
        end
    end
end
