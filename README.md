# Computational Burst Buffer: A New HPC Storage Paradigm and Its Concept Validation
This project is a simulation implementation of a computational burst buffer (CBB) based on a computational storage driver (CSD).
While preparing the artifacts, we deploy CBB prototype on a 16-node cluster using hardware CSDs with a realistic BB-to-PFS bandwidth ratio. Each node features two 2.4 GHz Intel Xeon Silver 4314 CPUs and 128 GB of DDR4 memory, running Ubuntu22.10 with the Linux 5.19.0 kernel.

## Minimum system requirements
Memory: >= 32 GB RAM

Processor: >= 8 cores (>=16 is recommended)

Storage: a SSD >= 1 TB, a HDD >= 1 TB, a CSD with compression engine >= 1 TB

gcc/9.5.0


## 1. Install
please make sure your installation directory is in the SSD.
### 1.1 Set environment

```
sed -i "1i export CBB_HOME=$(pwd)" environment.sh
source environment.sh
```
### 1.2 Install dependent libraries and packages (1 mins)
```
sudo apt install libtool automake autoconf make m4 grads default-jre csh time
cd $CBB_HOME/download
bash requirements.sh
```
### 1.3 Load or install MPICH (7 mins)

```
cd $CBB_HOME/download/
bash mpi.sh
```
### 1.4 Initial the BB config (1 mins)
```
cd $CBB_HOME/BB
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$DIR/grib2
make install
```
### 1.5 Download and install the HDF5 (2 mins)

```
cd $CBB_HOME/download
bash hdf5.sh
```
### 1.6 Install the SZ compressor (2 mins)
```
cd $CBB_HOME/Libs
bash compressor.sh
```
### 1.7 Install the WarpX (4 mins)
```
cd  $CBB_HOME/Application
bash warpx.sh
```
### 1.8 Install the FerroX (4 mins)
```
bash ferrox.sh
```
### 1.9 Install the Nyx (4 mins)
```
bash nyx.sh
```
### 1.10 Install the MAESTROeX (4 mins)
```
bash maestroex.sh
```

## 2. Download dataset and experiment

### 2.1 Download the checkpoint files of application
We pre-ran all application simulations, obtaining checkpoint files from later timesteps, to streamline the evaluation process for users. Additionally, The Ferrox application does not require input files. We have shared the checkpoint files of other applications on
OneDrive We share these file on Onedrive [checkpoint](https://hnueducn-my.sharepoint.com/:u:/g/personal/lbcs_hnu_edu_cn/EcyQZExgeVlAl-PL1YgKwwMB51CiaO11j9Y34ebzUHIJ7A?e=eHxOCc).

```
cp /your/install/checkpoint.tar.gz $CBB_HOME/workDir/
tar xvf checkpoint.tar.gz
```

### 2.2 Experimental evaluation
All evaluations are divided into 3 parts:
- A1 evaluates the CPU utilization of CBB compared to software compression
- A2 evaluates the overall performance of multiple representative real HPC applications under varying BB capacities
- A3 evaluates the performance difference between CBB and standard BB in complicated HPC workflow scenarios using a state-of-the-art simulator.
#### A1
```
cd $CBB_HOME/workDir/
bash A1.sh
cd $CBB_HOME/workDir/analysis/
bash A1-analysis.sh
```
#### A2
```
cd $CBB_HOME/workDir/
bash A2.sh
cd $CBB_HOME/workDir/analysis/
bash A2−analysis.sh
```
#### A3
```
cd $CBB_HOME/workDir/
bash A3.sh
cd $CBB_HOME/workDir/analysis/
bash A3−analysis.sh
```

