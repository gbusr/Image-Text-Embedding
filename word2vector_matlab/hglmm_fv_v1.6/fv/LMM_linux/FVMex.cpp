#include "mex.h" 
#include "FV.h"

#define NUM_OF_INPUTS 5
#define SAMPLES_INPUT_IDX 0
#define INIT_MU_INPUT_IDX 1
#define INIT_B_INPUT_IDX 2
#define INIT_PRIOR_INPUT_IDX 3
#define NORM_DESC_INPUT_IDX 4

#define FV_OUTPUT_IDX 0

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
	int nrhs, const mxArray *prhs[]) /* Input variables */
{
	if(nrhs != NUM_OF_INPUTS) /* Check the number of arguments */
		mexErrMsgTxt("Wrong number of input arguments.");
	
	double* samples = mxGetPr(prhs[SAMPLES_INPUT_IDX]);
	int numOfSamples = mxGetM(prhs[SAMPLES_INPUT_IDX]);
	int numOfDims = mxGetN(prhs[SAMPLES_INPUT_IDX]);

	double* mu = mxGetPr(prhs[INIT_MU_INPUT_IDX]);
	int classes = mxGetM(prhs[INIT_MU_INPUT_IDX]);

	int N = mxGetN(prhs[INIT_MU_INPUT_IDX]);
	if (N != numOfDims)
	{
		mexErrMsgTxt("Wrong number of dimensions for mu init.");
	}

	double* b = mxGetPr(prhs[INIT_B_INPUT_IDX]);
	int M = mxGetM(prhs[INIT_B_INPUT_IDX]);
	N = mxGetN(prhs[INIT_B_INPUT_IDX]);

	if (M != classes)
	{
		mexErrMsgTxt("Wrong number of classes for b init.");
	}

	if (N != numOfDims)
	{
		mexErrMsgTxt("Wrong number of dimensions for b init.");
	}

	double* priors = mxGetPr(prhs[INIT_PRIOR_INPUT_IDX]);
	M = mxGetM(prhs[INIT_PRIOR_INPUT_IDX]);
	
	if (M != classes)
	{
		mexErrMsgTxt("Wrong number of classes for prior init.");
	}

	fp_type** samples_for_algo;
	fp_type** mu_for_algo;
	fp_type** b_for_algo;
	fp_type* prior_for_algo;

	samples_for_algo = new fp_type*[numOfSamples];
	for (int i=0; i < numOfSamples; i++)
	{
		samples_for_algo[i] = new fp_type[numOfDims];
		for (int d=0; d < numOfDims; d++)
		{
			samples_for_algo[i][d] = samples[d*numOfSamples + i];
		}
	}

	mu_for_algo = new fp_type*[classes];
	for (int i=0; i < classes; i++)
	{
		mu_for_algo[i] = new fp_type[numOfDims];
	
		for (int d=0; d < numOfDims; d++)
		{
			mu_for_algo[i][d] = mu[d*classes + i];
		}
	}

	b_for_algo = new fp_type*[classes];
	for (int i=0; i < classes; i++)
	{
		b_for_algo[i] = new fp_type[numOfDims];
	
		for (int d=0; d < numOfDims; d++)
		{
			b_for_algo[i][d] = b[d*classes + i];
		}
	}

	prior_for_algo = new fp_type[classes];
	
	for (int i=0; i < classes; i++)
	{
		prior_for_algo[i] = priors[i];
	}

	double* normDesc_ptr =  mxGetPr(prhs[NORM_DESC_INPUT_IDX]);
	bool normDesc = normDesc_ptr[0];

	int outNumOfIterations = 0.0;
	
	fp_type* out_fv = new fp_type[2*numOfDims*classes];

	for (int j=0; j < (2*numOfDims*classes); j++)
	{
		out_fv[j]  = 0.0;
	}

	computeFV(numOfSamples, classes, numOfDims, samples_for_algo, mu_for_algo, b_for_algo, prior_for_algo, out_fv, normDesc);

	plhs[FV_OUTPUT_IDX] = mxCreateDoubleMatrix(2*numOfDims*classes, 1, mxREAL); 
	double* fvMexOut = mxGetPr(plhs[FV_OUTPUT_IDX]); 

	for (int j=0; j < (2*numOfDims*classes); j++)
	{
		fvMexOut[j]  = out_fv[j];
	}

	delete[] out_fv;

	for (int i=0; i < numOfSamples; i++)
	{
		delete samples_for_algo[i];
	}

	for (int c=0; c < classes; c++)
	{
		delete mu_for_algo[c];
		delete b_for_algo[c];
	}
	
	delete[] mu_for_algo;
	delete[] b_for_algo;
	delete[] samples_for_algo;
	delete[] prior_for_algo;

	return;
}