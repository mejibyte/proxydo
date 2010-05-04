task :default => [:compile]

task :clear do
  puts "Removing build directory..."
  system "rm -rf build"
  puts "Done"
end

task :compile do
  puts "Compiling..."
  begin
    Dir.mkdir "build"
  rescue
    puts "Apparently the build directory already exists. Maybe you want 'rake clear' first?"
    raise
  end
  Dir.chdir "src"
  system "g++ config.cpp main.cpp IncomingProxy.cpp OutgoingProxy.cpp sockets/ClientSocket.cpp sockets/ServerSocket.cpp sockets/Socket.cpp -o ../build/proxydo -I/usr/local/include/yaml-cpp -L/usr/local/lib -lyaml-cpp"
  puts "Everything compiled to /build"
  Dir.chdir ".."
end


task :all do
  Rake::Task["clear"].execute
  Rake::Task["compile"].execute
  system "cp config.yml.example build/config.yml"
end
