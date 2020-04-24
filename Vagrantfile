# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/xenial64"

  # config.vm.provider "virtualbox" do |vb|
  #   # Customize the amount of memory on the VM:
  #   vb.memory = "1024"
  # end

  config.vm.synced_folder ".", "/home/vagrant/bore"
  config.vm.synced_folder ".", "/vagrant", disabled: true

  config.vm.provision "shell", inline: <<-SHELL
    apt-get update
    apt-get upgrade

    apt-get install -y build-essential

    mkdir -p /tmp/lua
    cd /tmp/lua
    apt-get install -y libreadline-dev
    curl --silent -R -O http://www.lua.org/ftp/lua-5.3.5.tar.gz
    tar zxf lua-5.3.5.tar.gz
    cd lua-5.3.5
    make linux test
    make install
  SHELL

  config.vm.provision "shell", inline: <<-SHELL
    rm /etc/localtime
    sudo ln -s /usr/share/zoneinfo/US/Pacific /etc/localtime
  SHELL
end

