#!/usr/bin/env bash

# Add the latest stable Ubuntu xenial repositories for LLVM & Clang
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-7 main" | sudo tee -a /etc/apt/sources.list
echo "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-7 main" | sudo tee -a /etc/apt/sources.list

# Install required tools
apt-get update
apt-get install -y clang-7 libclang-7-dev python3 python3-pip

sudo pip3 install --upgrade pip
sudo pip3 install cmake --upgrade

# Configure Clang as default compiler
# Add convenience symlinks
sudo ln -s /usr/bin/clang-7 /usr/bin/clang
sudo ln -s /usr/bin/clang++-7 /usr/bin/clang++

# Set environment variables for the vagrant user
su vagrant -l -c "echo export CC=/usr/bin/clang | tee -a ~/.bashrc"
su vagrant -l -c "echo export CXX=/usr/bin/clang++ | tee -a ~/.bashrc"
su vagrant -l -c "source ~/.bashrc"
