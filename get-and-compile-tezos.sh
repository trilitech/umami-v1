#!/bin/sh

git clone -b alex@dry_run_no_pass git@gitlab.com:nomadic-labs/tezos.git .tezos
cd .tezos

rust_version=`rustc -V | cut -d' ' -f2`
echo "rust version echo $rust_version detected"
cp scripts/version.sh scripts/version.sh.old
sed -E "s/recommended_rust_version=(.+)/recommended_rust_version=$rust_version/" scripts/version.sh.old > scripts/version.sh

opam init --bare
make build-dev-deps

eval $(opam env)
make

tezos_path=`pwd`
export PATH=$tezos_path:$PATH
echo
echo "export PATH=\$PATH:$tezos_path to your shell rc file for future uses"

cd ..
