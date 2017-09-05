

unless Dir.exist?("gen/")
    mkdir_p "gen/"
end

IR = "scripts/ir"

Dir.chdir(IR) do
    sh "./ir-gen.rb"
end

copy_entry("#{IR}/output/", "gen/")
