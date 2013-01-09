# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant::Config.run do |config|
  config.vm.box = "precise64"

  config.vm.provision :shell, inline: <<-EOS
    sudo apt-get update
    sudo apt-get -y install build-essential vim curl libc6-dbg
    if ! valgrind --version | grep 3.8.1 -q; then
      curl -O http://valgrind.org/downloads/valgrind-3.8.1.tar.bz2
      tar -xjvf valgrind-3.8.1.tar.bz2
      cd ./valgrind-3.8.1
      ./configure
      make
      sudo make install
    fi
  EOS
end
