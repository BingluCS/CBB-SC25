#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sz.h"
#include "rw.h"
#include <sys/time.h>

struct timeval startTime;
struct timeval endTime;  /* Start and end times */
struct timeval costStart; /*only used for recording the cost*/
double totalCost = 0;


void cost_start()
{
	totalCost = 0;
	gettimeofday(&costStart, NULL);
}

void cost_end()
{
	double elapsed;
	struct timeval costEnd;
	gettimeofday(&costEnd, NULL);
	elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(costStart.tv_sec*1000000+costStart.tv_usec))/1000000.0;
	totalCost += elapsed;
}


void usage()
{
	printf("Usage: sz <options>\n");
	printf("Options:\n");
	printf("* operation type:\n");
	printf("	-z <compressed file>: the compression operation with an optionally specified output file.\n");
	printf("                          (the compressed file will be named as <input_file>.sz if not specified)\n");
	printf("	-x <decompressed file>: the decompression operation with an optionally specified output file\n");
	printf("                      (the decompressed file will be named as <cmpred_file>.out if not specified)\n");
	printf("	-p: print meta data (configuration info)\n");
	printf("	-h: print the help information\n");
	printf("	-v: print the version number\n");	
	printf("	-g: <n>: choose strategy to compress quantization factor. '0' for huffman (default), '1' for zstd and '2' for adt-fse.\n");
	printf("	-e: give a expected compression ratio of current file quickly and do not compress the entire file. 1D and 2D supported.\n");
	printf("* data type:\n");
	printf("	-f: single precision (float type)\n");
	printf("	-d: double precision (double type)\n");
	printf("* configuration file: \n");
	printf("	-c <configuration file> : configuration file sz.config\n");
	printf("* error control: (the error control parameters here will overwrite the setting in sz.config)\n");
	printf("	-M <error bound mode> : 10 options as follows. \n");
	printf("		ABS (absolute error bound)\n");
	printf("		REL (value range based error bound, so a.k.a., VR_REL)\n");
	printf("		ABS_AND_REL (using min{ABS, REL})\n");
	printf("		ABS_OR_REL (using max{ABS, REL})\n");
	printf("		PSNR (peak signal-to-noise ratio)\n");
	printf("		NORM (norm2 error : sqrt(sum(xi-xi')^2)\n"); 
	printf("		PW_REL (point-wise relative error bound)\n");
	printf("	-A <absolute error bound>: specifying absolute error bound\n");
	printf("	-R <value_range based relative error bound>: specifying relative error bound\n");
	printf("	-P <point-wise relative error bound>: specifying point-wise relative error bound\n");
	printf("	-S <PSNR>: specifying PSNR\n");
	printf("	-N <normErr>: specifying normErr\n");
	printf("* input data file:\n");
	printf("	-i <original data file> : original data file\n");
	printf("	-s <compressed data file> : compressed data file in decompression\n");
	printf("* output type of decompressed file: \n");
	printf("	-b (by default) : decompressed file stored in binary format\n");
	printf("	-t : decompreadded file stored in text format\n");
	printf("	-T : pre-processing with Tucker Tensor Decomposition\n");
	printf("* dimensions: \n");
	printf("	-1 <nx> : dimension for 1D data such as data[nx]\n");
	printf("	-2 <nx> <ny> : dimensions for 2D data such as data[ny][nx]\n");
	printf("	-3 <nx> <ny> <nz> : dimensions for 3D data such as data[nz][ny][nx] \n");
	printf("	-4 <nx> <ny> <nz> <np>: dimensions for 4D data such as data[np][nz][ny][nx] \n");
	printf("* print compression results: \n");
	printf("	-a : print compression results such as distortions\n");
#ifdef HAVE_WRITESTATS
	printf("	-q : print the stats about the compressor (only for compression)\n");
#endif
	printf("* examples: \n");
	printf("	sz -z -f -c sz.config -i testdata/x86/testfloat_8_8_128.dat -3 8 8 128\n");
	printf("	sz -z -f -c sz.config -M ABS -A 1E-3 -i testdata/x86/testfloat_8_8_128.dat -3 8 8 128\n");
	printf("	sz -x -f -s testdata/x86/testfloat_8_8_128.dat.sz -3 8 8 128\n");
	printf("	sz -x -f -s testdata/x86/testfloat_8_8_128.dat.sz -i testdata/x86/testfloat_8_8_128.dat -3 8 8 128 -a\n");	
	printf("	sz -z -d -c sz.config -i testdata/x86/testdouble_8_8_128.dat -3 8 8 128\n");
	printf("	sz -x -d -s testdata/x86/testdouble_8_8_128.dat.sz -3 8 8 128\n");
	printf("	sz -p -s testdata/x86/testdouble_8_8_128.dat.sz\n");
	exit(0);
}


int main(int argc, char* argv[])
{
	int binaryOutput = 1;
	int printCmpResults = 0;
#ifdef HAVE_WRITESTATS	
	int printStats = 0;
#endif
	int isCompression = -1000; //1 : compression ; 0: decompression
	int printMeta = 0;
	int dataType = 0; //0: single precision ; 1: double precision
	int tucker = 0; //0: without tucker tensor decomposition preprocessing; 1: with tucker tensor decomposition
	int entropyType = 0;
	int prediction_ = 0;
	char* inPath = NULL;
	char* cmpPath = NULL;
	char* conPath = NULL;
	char* decPath = NULL;
	
	char* errBoundMode = NULL;
	char* absErrorBound = NULL;
	char* relErrorBound = NULL;
	char* pwrErrorBound = NULL;
	char* psnr_ = NULL;
	char* normError = NULL;
	
	size_t r5 = 0;
	size_t r4 = 0;
	size_t r3 = 0;
	size_t r2 = 0; 
	size_t r1 = 0;
	
	size_t i = 0;
	int status;
	size_t nbEle;
	if(argc==1)
		usage();
	
	for(i=1;i<argc;i++)
	{
		if (argv[i][0] != '-' || argv[i][2])
			usage();
		switch (argv[i][1])
		{
		case 'h':
			usage();
			exit(0);
		case 'v':
			printf("version: %d.%d.%d\n", SZ_VER_MAJOR, SZ_VER_MINOR, SZ_VER_BUILD);
			exit(0);
		case 'b': 
			binaryOutput = 1;
			break;
		case 't': 
			binaryOutput = 0;
			break;
		case 'a':
			printCmpResults = 1;
			break;
#ifdef HAVE_WRITESTATS	
		case 'q':
			printStats = 1;
			break;
#endif			
		case 'g':
			if (++i == argc || sscanf(argv[i], "%u", &entropyType) != 1) {
				usage();
				exit(0);
			}
			break;
		case 'e':
			prediction_ = 1;
			break;
		case 'z':
			isCompression = 1;
			if (i+1 < argc)
			{
				cmpPath = argv[i+1];
				if(cmpPath[0]!='-')
					i++;
				else
					cmpPath = NULL;
			}
			break;
		case 'x': 
			isCompression = 0;
			if (i+1 < argc)
			{
				decPath = argv[i+1];
				if(decPath[0]!='-')
					i++;
				else
					decPath = NULL;
			}			
			break;
		case 'p':
			printMeta = 1; //print metadata
			break;			
		case 'f': 
			dataType = 0;
			break;
		case 'd':
			dataType = 1;
			break;
		case 'i':
			if (++i == argc)
				usage();
			inPath = argv[i];		
			break;
		case 's':
			if (++i == argc)
				usage();
			cmpPath = argv[i];
			break;
		case 'c':
			if (++i == argc)
				usage();
			conPath = argv[i];
			break;
		case 'T':
			tucker = 1;
			break;
		case '1': 
			if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1)
				usage();
			break;
		case '2':
			if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r2) != 1)
				usage();
			break;
		case '3':
			if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r2) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r3) != 1)
				usage();		
			break;
		case '4':
			if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r2) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r3) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &r4) != 1)
				usage();		
			break;
		case 'M':
			if (++i == argc)
				usage();
			errBoundMode = argv[i];
			break;
		case 'A':
			if (++i == argc)
				usage();
			absErrorBound = argv[i];
			break;
		case 'R':
			if (++i == argc)
				usage();
			relErrorBound = argv[i];
			break;
		case 'P':
			if (++i == argc)
				usage();
			pwrErrorBound = argv[i];
			break;
		case 'N':
			if (++i == argc)
				usage();
			normError = argv[i];
			break;
		case 'S': 
			if (++i == argc)
				usage();
			psnr_ = argv[i];
			break;
		default: 
			usage();
			break;
		}
	}

	if((inPath==NULL) & (cmpPath == NULL))
	{
		printf("Error: you need to specify either a raw binary data file or a compressed data file as input\n");
		usage();
		exit(0);
	}

	if(printMeta == 0)
	{
		if ((r1==0) && (r2==0) && (r3==0) && (r4==0) && (r5==0))
		{
			printf ("Error: please specify dimensions.\n");
			printf("-1 <nx> : dimension for 1D data such as data[nx]\n");
			printf("-2 <nx> <ny> : dimensions for 2D data such as data[ny][nx]\n");
			printf("-3 <nx> <ny> <nz> : dimensions for 3D data such as data[nz][ny][nx] \n");
			printf("-4 <nx> <ny> <nz> <np>: dimensions for 4D data such as data[np][nz][ny][nx] \n");
			exit(0);
		}		
	}
	else
	{
		if(cmpPath == NULL && isCompression != 1) //if no compression file is provided and this is not a compression operation
		{
			printf("Error: -p can only be used when providing a compressed data file or in the compression step\n");
			printf("Solution: use -s to specify a compressed data file or use -c and -i to generate a compressed file\n");
			usage();
			exit(0);
		}
	}
	
	//Initialization (only for compression because decompression doesn't need the initialization)
	if(isCompression == 1)
	{
		if(SZ_NSCS==SZ_Init(conPath))
			exit(0);
	}
	if(isCompression == 1 && errBoundMode != NULL)
	{
		int errorBoundMode = 0;
		if(strcmp(errBoundMode, "ABS")==0)
			errorBoundMode = ABS;
		else if(strcmp(errBoundMode, "REL")==0||strcmp(errBoundMode, "VR_REL")==0)
			errorBoundMode = REL;
		else if(strcmp(errBoundMode, "ABS_AND_REL")==0)
			errorBoundMode = ABS_AND_REL;
		else if(strcmp(errBoundMode, "ABS_OR_REL")==0)
			errorBoundMode = ABS_OR_REL;
		else if(strcmp(errBoundMode, "PSNR")==0)
			errorBoundMode = PSNR;
		else if(strcmp(errBoundMode, "PW_REL")==0)
			errorBoundMode = PW_REL;
		else if(strcmp(errBoundMode, "NORM")==0)
			errorBoundMode = NORM;
		else
		{
			printf("Error: wrong error bound mode setting by using the option '-M'\n");
			usage();
			exit(0);
		}
		confparams_cpr->errorBoundMode = errorBoundMode;
	}
	
	char outputFilePath[256];	
	unsigned char *bytes = NULL; //the binary data read from "compressed data file"
	size_t byteLength = 0; 
	if(isCompression == 1)
	{
		confparams_cpr->entropy_type = entropyType;
		confparams_cpr->prediction = prediction_;
		if(absErrorBound != NULL)
			confparams_cpr->absErrBound = atof(absErrorBound);
		
		if(relErrorBound != NULL)
			confparams_cpr->relBoundRatio = atof(relErrorBound);
	
		if(pwrErrorBound != NULL)
			confparams_cpr->pw_relBoundRatio = atof(pwrErrorBound);
	
		if(psnr_ != NULL)
			confparams_cpr->psnr = atof(psnr_);

		if(normError != NULL)
			confparams_cpr->normErr = atof(normError);

		if(dataType == 0) //single precision
		{
			if(tucker)
			{
				printf("Error: Single-precision Tucker tensor decomposition is not supported by TuckerMPI yet. \n");
				printf("Solution: change the data format to be double-precision and then do the tensor decomposition.\n");
				exit(0);
			}

			float *data = readFloatData(inPath, &nbEle, &status);
			if(status!=SZ_SCES)
			{
				printf("Error: cannot read the input file: %s\n", inPath);
				exit(0);
			}
			cost_start();	
			if(confparams_cpr->sol_ID==SZ)
				bytes = SZ_compress(SZ_FLOAT, data, &byteLength, r5, r4, r3, r2, r1);
			else if(confparams_cpr->sol_ID==SZ_Transpose)
			{
				int status = 0;
				bytes = SZ_compress_customize("SZ_Transpose", NULL, SZ_FLOAT, data, r5, r4, r3, r2, r1, &byteLength, &status);
			}	
			cost_end();
			if(cmpPath == NULL)
				sprintf(outputFilePath, "%s.sz", inPath);
			else
				strcpy(outputFilePath, cmpPath);
			writeByteData(bytes, byteLength, outputFilePath, &status);		
			free(data);
			if(status != SZ_SCES)
			{
				printf("Error: data file %s cannot be written!\n", outputFilePath);
				exit(0);
			}
			printf("compression time = %f\n", totalCost);
			// printf("compressed data file: %s\n", outputFilePath);			

#ifdef HAVE_WRITESTATS	
			if(printStats)
				printSZStats();		
#endif					
		}
		else //dataType == 1: double precision
		{
			if(tucker)
			{
				const char* s = getenv("TUCKERMPI_PATH");
				if(s==NULL)
				{
					printf("Error: the environment variable TUCKERMPI_PATH == NULL. \n");
					printf("Solution: Install TuckerMPI and set environment variable TUCKERMPI_HOME to the building path (e.g., TuckerMPI-gitlab/build)\n"); 
					exit(0);
				}	
				
				//TODO: constructing the parameter-raw.txt	
				char *str[8] = {
					"Automatic rank determination = true", 
					"Perform STHOSVD = true", 
					"Write STHOSVD result = true", 
					"Print options = true", 
					NULL, 
					"Scaling type = StandardCentering", 
					"Scale mode = 2", 
					NULL};
							
				char dimStr[256];
				if(r2==0)
					sprintf(dimStr, "Global dims = %zu", r1);
				else if(r3==0)
					sprintf(dimStr, "Global dims = %zu %zu", r2, r1);
				else if(r4==0)
					sprintf(dimStr, "Global dims = %zu %zu %zu", r3, r2, r1);
				else if(r5==0)
					sprintf(dimStr, "Global dims = %zu %zu %zu %zu", r4, r3, r2, r1);
				else
					sprintf(dimStr, "Global dims = %zu %zu %zu %zu %zu", r5, r4, r3, r2, r1);
				
				str[4] = dimStr;
				
				char thrStr[100]; 
				sprintf(thrStr, "SV Threshold = %f", confparams_cpr->absErrBound);
				str[7] = thrStr;

				writeStrings(8, str, "parameter-raw.txt", &status);	

				//TODO: constructing the raw.txt (containing the path of the binary data file
				char* dataPathStr[1];
				dataPathStr[0] = inPath;
				writeStrings(1, dataPathStr, "raw.txt", &status);
				
				printf("calling TuckerMPI interface to do the Tucker Tensor Decomposition....\n");
				
				system("mkdir -p ./compressed");
				system("${TUCKERMPI_PATH}/serial/drivers/bin/Tucker_sthosvd --parameter-file parameter-raw.txt");
			}
			else
			{
				double *data = readDoubleData(inPath, &nbEle, &status);	
				if(status!=SZ_SCES)
				{
					printf("Error: cannot read the input file: %s\n", inPath);
					exit(0);
				}
				cost_start();
				if(confparams_cpr->sol_ID==SZ)
					bytes = SZ_compress(SZ_DOUBLE, data, &byteLength, r5, r4, r3, r2, r1);
				else if(confparams_cpr->sol_ID==SZ_Transpose)
				{
					int status = 0;
					bytes = SZ_compress_customize("SZ_Transpose", NULL, SZ_DOUBLE, data, r5, r4, r3, r2, r1, &byteLength, &status);
				}				
				cost_end();
				if(cmpPath == NULL)
					sprintf(outputFilePath, "%s.sz", inPath);
				else
					strcpy(outputFilePath, cmpPath);
				writeByteData(bytes, byteLength, outputFilePath, &status);		
				free(data);
				if(status != SZ_SCES)
				{
					printf("Error: data file %s cannot be written!\n", outputFilePath);
					exit(0);
				}		
				printf("compression time = %f\n", totalCost);
				// printf("compressed data file: %s\n", outputFilePath);
			}	
		}

		if (printCmpResults == 1)
		{
			printf ("Error: -a can be only used in decompression.\n");
		}
	}
	else if(isCompression == 0) //decompression
	{
		if(printCmpResults)
		{
			if(inPath==NULL)
			{
				printf("Error: Since you add -a option (analysis), please specify the original data path by -i <path>.\n");
				exit(0);
			}
		}		
		
		char outputFilePath[256];
		
		if(r2==0)
			nbEle = r1;
		else if(r3==0)
			nbEle = r1*r2;
		else if(r4==0)
			nbEle = r1*r2*r3;
		else if(r5==0)
			nbEle = r1*r2*r3*r4;
		else
			nbEle = r1*r2*r3*r4*r5;

		if(checkFileExistance(cmpPath)==0)
		{
			printf("Error: compression file (%s) is not readable.\n", cmpPath);
			exit(0);
		}

		if(dataType == 0)
		{
			if(tucker)
			{
				printf("Error: Single-precision Tucker tensor decomposition is not supported by TuckerMPI yet. \n");
				printf("Solution: change the data format to be double-precision and then do the tensor decomposition.\n");
				exit(0);
			}			
			
			bytes = readByteData(cmpPath, &byteLength, &status);
			if(status!=SZ_SCES)
			{
				printf("Error: %s cannot be read!\n", cmpPath);
				exit(0);
			}
			cost_start();
			float *data = NULL;
			float *data_ = SZ_decompress(SZ_FLOAT, bytes, byteLength, r5, r4, r3, r2, r1);
			if(confparams_dec->sol_ID==SZ_Transpose)
				data = detransposeData(data_, SZ_FLOAT, r5, r4, r3, r2, r1);
			else //confparams_dec->sol_ID==SZ
				data = data_;
			cost_end();
			if(decPath == NULL)
				sprintf(outputFilePath, "%s.out", cmpPath);	
			else
				strcpy(outputFilePath, decPath);
			if(binaryOutput==1)		
				writeFloatData_inBytes(data, nbEle, outputFilePath, &status);
			else //txt output
				writeFloatData(data, nbEle, outputFilePath, &status);

			if(status!=SZ_SCES)
			{
				printf("Error: %s cannot be written!\n", outputFilePath);
				exit(0);
			}
			
			if(printCmpResults)
			{
				if(inPath==NULL)
				{
					printf("Error: Since you add -a option (analysis), please specify the original data path by -i <path>.\n");
					exit(0);
				}
				//compute the distortion / compression errors...
				size_t totalNbEle;
				float *ori_data = readFloatData(inPath, &totalNbEle, &status);
				if(status!=SZ_SCES)
				{
					printf("Error: %s cannot be read!\n", inPath);
					exit(0);
				}

				size_t i = 0;
				float Max = 0, Min = 0, diffMax = 0;
				Max = ori_data[0];
				Min = ori_data[0];
				diffMax = fabs(data[0] - ori_data[0]);
				double sum1 = 0, sum2 = 0, sum22 = 0;
				for (i = 0; i < nbEle; i++)
				{
					sum1 += ori_data[i];
					sum2 += data[i];
					sum22 += data[i]*data[i];
				}
				double mean1 = sum1/nbEle;
				double mean2 = sum2/nbEle;

				double sum3 = 0, sum4 = 0;
				double sum = 0, prodSum = 0, relerr = 0;

				double maxpw_relerr = 0; 
				for (i = 0; i < nbEle; i++)
				{
					if (Max < ori_data[i]) Max = ori_data[i];
					if (Min > ori_data[i]) Min = ori_data[i];
					
					float err = fabs(data[i] - ori_data[i]);
					if(ori_data[i]!=0)
					{
						relerr = err/fabs(ori_data[i]);
						//float rerr = err/0.0020479534287005662918;
						//if(rerr>0.001)
						//{printf("i=%zu, %.30G\n", i, relerr); break;}
						if(maxpw_relerr<relerr)
							maxpw_relerr = relerr;
					}

					if (diffMax < err)
						diffMax = err;
					prodSum += (ori_data[i]-mean1)*(data[i]-mean2);
					sum3 += (ori_data[i] - mean1)*(ori_data[i]-mean1);
					sum4 += (data[i] - mean2)*(data[i]-mean2);
					sum += err*err;	
				}
				double std1 = sqrt(sum3/nbEle);
				double std2 = sqrt(sum4/nbEle);
				double ee = prodSum/nbEle;
				double acEff = ee/std1/std2;

				double mse = sum/nbEle;
				double range = Max - Min;
				double psnr = 20*log10(range)-10*log10(mse);
				double nrmse = sqrt(mse)/range;
				double compressionRatio = 1.0*nbEle*sizeof(float)/byteLength;
				double normErr = sqrt(sum);
				double normErr_norm = normErr/sqrt(sum22);		
		
				printf ("Min=%.20G, Max=%.20G, range=%.20G\n", Min, Max, range);
				printf ("Max absolute error = %.10f\n", diffMax);
				printf ("Max relative error = %f\n", diffMax/(Max-Min));
				printf ("Max pw relative error = %f\n", maxpw_relerr);
				printf ("PSNR = %f, NRMSE= %.20G\n", psnr,nrmse);
				printf ("normError = %f, normErr_norm = %f\n", normErr, normErr_norm);
				printf ("acEff=%f\n", acEff);	
				printf ("compressionRatio=%f\n", compressionRatio);
				
				free(ori_data);
			}
			free(data);	
			
			printf("decompression time = %f seconds.\n", totalCost);
			// printf("decompressed data file: %s\n", outputFilePath);							
		}
		else //double-data
		{
			double *data = NULL;
			if(tucker)
			{
				const char* s = getenv("TUCKERMPI_PATH");
				if(s==NULL)
				{
					printf("Error: the environment variable TUCKERMPI_PATH == NULL. \n");
					printf("Solution: Install TuckerMPI and set environment variable TUCKERMPI_HOME to the building path (e.g., TuckerMPI-gitlab/build)\n"); 
					exit(0);
				}	
				
				//TODO: constructing the parameter-raw.txt	
				char *str[4] = {
					"Print options = true", 
					NULL, 
					NULL, 
					"STHOSVD directory = ./compressed"};
				char dimStr1[256];
				if(r2==0)
					sprintf(dimStr1, "Beginning subscripts = 0");
				else if(r3==0)
					sprintf(dimStr1, "Beginning subscripts = 0 0");
				else if(r4==0)
					sprintf(dimStr1, "Beginning subscripts = 0 0 0");
				else if(r5==0)
					sprintf(dimStr1, "Beginning subscripts = 0 0 0 0");
				else
					sprintf(dimStr1, "Beginning subscripts = 0 0 0 0 0");
				
				str[1] = dimStr1;
						
				char dimStr2[256];
				if(r2==0)
					sprintf(dimStr2, "Ending subscripts = %zu", r1-1);
				else if(r3==0)
					sprintf(dimStr2, "Ending subscripts = %zu %zu", r2-1, r1-1);
				else if(r4==0)
					sprintf(dimStr2, "Ending subscripts = %zu %zu %zu", r3-1, r2-1, r1-1);
				else if(r5==0)
					sprintf(dimStr2, "Ending subscripts = %zu %zu %zu %zu", r4-1, r3-1, r2-1, r1-1);
				else
					sprintf(dimStr2, "Ending subscripts = %zu %zu %zu %zu %zu", r5-1, r4-1, r3-1, r2-1, r1-1);
				
				str[2] = dimStr2;

				writeStrings(4, str, "parameter-rec.txt", &status);		

				//TODO: constructing the raw.txt (containing the path of the binary data file				
				strcpy(outputFilePath, "tucker-decompress.out");
				char* dataPathStr[1];
				dataPathStr[0] = outputFilePath;
				writeStrings(1, dataPathStr, "rec.txt", &status);
				
				printf("calling TuckerMPI interface to do the Tucker Tensor Decomposition....\n");
				
				system("${TUCKERMPI_PATH}/serial/drivers/bin/Tucker_reconstruct --parameter-file parameter-rec.txt");
			}
			else
			{
				bytes = readByteData(cmpPath, &byteLength, &status);
				if(status!=SZ_SCES)
				{
					printf("Error: %s cannot be read!\n", cmpPath);
					exit(0);
				}
				cost_start();
				double *data_ = SZ_decompress(SZ_DOUBLE, bytes, byteLength, r5, r4, r3, r2, r1);
				if(confparams_dec->sol_ID==SZ_Transpose)
					data = detransposeData(data_, SZ_DOUBLE, r5, r4, r3, r2, r1);
				else //confparams_dec->sol_ID==SZ
					data = data_;				

				cost_end();
				if(decPath == NULL)
					sprintf(outputFilePath, "%s.out", cmpPath);	
				else
					strcpy(outputFilePath, decPath);
				if(binaryOutput==1)		
				  writeDoubleData_inBytes(data, nbEle, outputFilePath, &status);
				else //txt output
				  writeDoubleData(data, nbEle, outputFilePath, &status);			
				if(status!=SZ_SCES)
				{
					printf("Error: %s cannot be written!\n", outputFilePath);
					exit(0);
				}
						
				printf("decompression time = %f seconds.\n", totalCost);
				// printf("decompressed data file: %s\n", outputFilePath);										
			}
			
			
			if(printCmpResults)
			{
				if(inPath==NULL)
				{
					printf("Error: Since you add -a option (analysis), please specify the original data path by -i <path>.\n");
					exit(0);
				}
				size_t totalNbEle;

				if(tucker)
					data = readDoubleData("tucker-decompress.out", &totalNbEle, &status);

				//compute the distortion / compression errors...
				double *ori_data = readDoubleData(inPath, &totalNbEle, &status);
				if(status!=SZ_SCES)
				{
					printf("Error: %s cannot be read!\n", inPath);
					exit(0);
				}

				size_t i = 0;
				double Max = 0, Min = 0, diffMax = 0;
				Max = ori_data[0];
				Min = ori_data[0];
				diffMax = data[0]>ori_data[0]?data[0]-ori_data[0]:ori_data[0]-data[0];

				//diffMax = fabs(data[0] - ori_data[0]);
				double sum1 = 0, sum2 = 0, sum22 = 0;

				for (i = 0; i < nbEle; i++)
				{
					sum1 += ori_data[i];
					sum2 += data[i];
					sum22 += data[i]*data[i];
				}
				double mean1 = sum1/nbEle;
				double mean2 = sum2/nbEle;

				double sum3 = 0, sum4 = 0;
				double sum = 0, prodSum = 0, relerr = 0;

				double maxpw_relerr = 0; 
				for (i = 0; i < nbEle; i++)
				{
					if (Max < ori_data[i]) Max = ori_data[i];
					if (Min > ori_data[i]) Min = ori_data[i];

					float err = fabs(data[i] - ori_data[i]);
					if(ori_data[i]!=0)
					{
						relerr = err/fabs(ori_data[i]);
						if(maxpw_relerr<relerr)
						  maxpw_relerr = relerr;
					}

					if (diffMax < err)
					  diffMax = err;
					prodSum += (ori_data[i]-mean1)*(data[i]-mean2);
					sum3 += (ori_data[i] - mean1)*(ori_data[i]-mean1);
					sum4 += (data[i] - mean2)*(data[i]-mean2);
					sum += err*err;	
				}
				double std1 = sqrt(sum3/nbEle);
				double std2 = sqrt(sum4/nbEle);
				double ee = prodSum/nbEle;
				double acEff = ee/std1/std2;

				double mse = sum/nbEle;
				double range = Max - Min;
				double psnr = 20*log10(range)-10*log10(mse);
				double normErr = sqrt(sum);
				double normErr_norm = normErr/sqrt(sum22);
				double nrmse = sqrt(mse)/range;

				double compressionRatio = 1.0*nbEle*sizeof(double)/byteLength;

				printf ("Min = %.20G, Max = %.20G, range = %.20G\n", Min, Max, range);
				printf ("Max absolute error = %.10f\n", diffMax);
				printf ("Max relative error = %f\n", diffMax/(Max-Min));
				printf ("Max pw relative error = %f\n", maxpw_relerr);
				printf ("PSNR = %f, NRMSE = %.20G\n", psnr,nrmse);
				printf ("normErr = %f, normErr_norm = %f\n", normErr, normErr_norm);
				printf ("acEff = %f\n", acEff);
				printf ("compressionRatio = %f\n", compressionRatio);
				
				free(ori_data);
			}			
			free(data);								
		}	
	}
	
	if(printMeta==1) //==-1 for printing metadata
	{
		int status;
		if(bytes==NULL)
			bytes = readByteData(cmpPath, &byteLength, &status);
			
		int szMode = 0;
		unsigned char* bytes2 = NULL;
		int losslessCompressor = is_lossless_compressed_data(bytes, byteLength);
		if(losslessCompressor!=-1)
		{
			szMode = SZ_BEST_COMPRESSION;
			sz_lossless_decompress65536bytes(losslessCompressor, bytes, (unsigned long)byteLength, &bytes2);	
		}
		else
		{
			szMode = SZ_BEST_SPEED;	
			bytes2 = bytes;
		}				
			
		sz_metadata* metadata = SZ_getMetadata(bytes2);
		metadata->conf_params->szMode = szMode;

		if(metadata->versionNumber[0]==0)
		{
			printf("Error: the compressed data file is likely wrong.\n");
			usage();
			free(metadata->conf_params);
			free(metadata);
			exit(0);
		}
		SZ_printMetadata(metadata);
		free(metadata->conf_params);
		confparams_dec = NULL;
		free(metadata);
		
		if(losslessCompressor!=-1)
			free(bytes2);
	}
	else 
	{
		if(isCompression<0)
		{
			printf("Error: confusing option. the option of operation cannot be missing. \n");
			printf("Hint: please specify the operation using '-z', '-x', '-p', or '-h'.\n");
			usage();
		}
	}
	
	free(bytes);
	
	SZ_Finalize();
}
