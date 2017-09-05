

IR = "scripts/ir"

Dir.chdir(IR) do
    if ARGV.include? "clean"
        sh "rake clean"
    else
        sh "rake"
    end
end

def make_copy_task(src, dst)

    dir = File.dirname(dst)

    file(dst => dir)
    directory(dir)

    file(dst => src) do
        copy(src, dst)
    end

    task :default => dst
end

unless ARGV.include? "clean"

    IR_OUT = "#{IR}/output"
    IR_SRC = FileList["#{IR_OUT}/**/*.*"]
    IR_DST = IR_SRC.pathmap("%{^#{IR_OUT}/,gen/}p")

    IR_SRC.zip(IR_DST).each do |src, dst|
        make_copy_task(src, dst)
    end
end

task :clean do
    rm_rf "gen/"
end
