

require "colorize"


def src_deps(src, flags)
    output = `gcc -MM #{flags.join(" ")} #{src}`
    return output.split(" ").slice(1..-1)
end


def make_compile_task(src, obj, flags, extra_deps = [])

    obj_dir = File.dirname(obj)

    file(obj => obj_dir)
    directory(obj_dir)
    file(obj => src)

    file(obj => src_deps(src, flags))
    file(obj => extra_deps)

    file(obj) do
        sh "gcc #{flags.join(" ")} #{src} -o #{obj}"
    end
end


def make_link_task(bin, objs, flags, extra_deps = [])

    bin_dir = File.dirname(bin)

    file(bin => bin_dir)
    directory(bin_dir)
    file(bin => objs)

    file(bin => extra_deps)

    file(bin) do
        sh "gcc #{flags.join(" ")} #{objs.join(" ")} -o #{bin}"
    end
end


def run_tests(bins)

    puts
    puts "Running tests...".colorize(:green)
    puts

    some_failed = false

    bins.each do |bin|
        if system(bin)
            puts "#{bin}".colorize(:yellow)
        else
            puts "#{bin}".colorize(:red)
            some_failed = true
        end
    end

    puts
    if some_failed
        puts "some tests failed.".colorize(:red)
    else
        puts "all tests passed.".colorize(:green)
    end
end
