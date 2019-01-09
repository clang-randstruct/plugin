Vagrant.configure("2") do |config|
    config.vm.box = "generic/ubuntu1810"
    config.vm.provision :shell, path: "bootstrap.sh"
    config.vm.synced_folder ".", "/vagrant", disabled: true
    config.vm.synced_folder ".", "/clang-randstruct"
end
