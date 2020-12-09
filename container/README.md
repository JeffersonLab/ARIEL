## ARIEL containers

This directory contains a [definition file](ARIEL.def)
for building a [Singularity](https://sylabs.io/singularity) container
for ARIEL. It has been tested with Singularity 3.5 and 3.6.

This container is based on Ubuntu 20.04 LTS. The following main software components
will be installed from source:

* ARIEL 0.6.4 (art 3.06.03)
* [ROOT 6.22/06](https://root.cern/doc/v622/release-notes.html#release-6.2206)
* Vc and VecGeom libraries for ROOT
* Pythia 8.3.0.3

The container can be built on any Linux host that supports Singularity 3.5+.
Examples for Debian/Ubuntu and CentOS/RHEL/Fedora follow.

### Debian/Ubuntu

Debian/Ubuntu distributions currently do not offer a recent version of
Singularity in the standard repositories. It is therefore best installed from source.
See the detailed instructions
[here](https://sylabs.io/guides/3.6/admin-guide/installation.html#installation-on-linux).

Here is a working example for Ubuntu 20.04. 

~~~~~~~~~~bash
sudo apt-get update
sudo apt-get install -y golang build-essential uuid-dev libgpgme-dev \
    squashfs-tools libseccomp-dev wget pkg-config git cryptsetup-bin
cd /tmp
# Update version number as appropriate
export VERSION=3.6.4
wget https://github.com/sylabs/singularity/releases/download/v${VERSION}/singularity-${VERSION}.tar.gz 
tar -xzf singularity-${VERSION}.tar.gz
cd singularity
./mconfig
make -j$(nproc) -C builddir
sudo make -C builddir install
cd
rm -rf /tmp/singularity /tmp/singularity-${VERSION}.tar.gz
singularity --version
singularity version 3.6.4
~~~~~~~~~~

With Singularity installed, see [below](#building-container) for how to build the
ARIEL container.

### CentOS/RHEL

First enable the [EPEL](https://fedoraproject.org/wiki/EPEL) repository.
Then install the following packages

~~~~~~~~~~bash
yum install singularity debootstrap
~~~~~~~~~~

Next, see [below](#building-container) for how to build the ARIEL container.

### Fedora

On Fedora (version 30+), one can install the required packages directly
from the system repositories:

~~~~~~~~~~bash
dnf install singularity debootstrap
~~~~~~~~~~

### Building the ARIEL container<a name="building-container"></a>

Building the ARIEL Singularity container is straightforward, with one
exception. For a full OS install inside the container, Singularity
requires `suid` and `dev` permissions on its temporary working
directory, which is `/tmp` by default. `/tmp`, in turn, is usually
mounted with `nodev` and possibly also `nosuid`, depending on the host
configuration.  To work around this, it is easiest to tell Singularity
to use a temporary directory on the `/home` filesystem.  The following
should work on any of the distributions referenced above.

~~~~~~~~~~bash
mkdir -p $HOME/Singularity/tmp
cd $HOME/Singularity
export SINGULARITY_TMPDIR=$(pwd)/tmp
sudo -E singularity build ARIEL.sif ARIEL.def
~~~~~~~~~~

Depending on the host system, this may take anywhere between
approximately 15 and 120 minutes. The build process will use all
available CPU threads. 1 GB of RAM per thread and 5 GB of free disk
space on `/home` are recommended.

### Using the container

See the built-in help to get started

```bash
singularity run-help ARIEL.sif
```
