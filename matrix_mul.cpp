
#define lm 3
#define ln 3
#define lp 3
#define M (1<<lm)
#define N (1<<ln)
#define P (1<<lp)



extern "C" {
void vadd(const unsigned int *in1, // Read-Only Vector 1
          const unsigned int *in2, // Read-Only Vector 2
          unsigned int *out_r ,    // Output Result
          int size                 // Size in integer
          ) {

// Here Vitis kernel contains one s_axilite interface which will be used by host
// application to configure the kernel.
// Here bundle control is defined which is s_axilite interface and associated
// with all the arguments (in1, in2, out_r and size),
// control interface must also be associated with "return".
// All the global memory access arguments must be associated to one m_axi(AXI
// Master Interface). Here all three arguments(in1, in2, out_r) are
// associated to bundle gmem which means that a AXI master interface named
// "gmem" will be created in Kernel and all these variables will be
// accessing global memory through this interface.
// Multiple interfaces can also be created based on the requirements. For
// example when multiple memory accessing arguments need access to
// global memory simultaneously, user can create multiple master interfaces and
// can connect to different arguments.
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_r bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

		unsigned int BRAM_in1[N][M];
		unsigned int BRAM_in2[M][P];
		unsigned int BRAM_out_r[N][P];
		unsigned int f = M / 2;

	#pragma HLS ARRAY_PARTITION variable=BRAM_in1 cyclic factor=f dim=2
	#pragma HLS ARRAY_PARTITION variable=BRAM_in2 cyclic factor=f dim=1
		int i,j,k;

		//TRANSFER DATA TO BUFFERS
		for(i=0;i<N;i++){
				for(j=0;j<M;j++){
		#pragma HLS PIPELINE II=1
						BRAM_in1[i][j]=in1[i*M+j];
				}
		}
		for(i=0;i<M;i++){
				for(j=0;j<P;j++){
		#pragma HLS PIPELINE II=1
						BRAM_in2[i][j]=in2[i*P+j];
				}
		}

		//CALCULATE THE RESULT
		for(int i = 0; i < N; i++){
		        for(int j = 0; j < P; j++){
		#pragma HLS PIPELINE II=1
		            int result=0;
		            for(int k = 0; k < M; k++){
		                result += (BRAM_in1[i][k] * BRAM_in2[k][j]);
		            }
		            BRAM_out_r[i][j] = result;
		        }
		    }

		//COPY RESULT FROM BUFFER TO C
		for(i=0;i<N;i++){
				for(j=0;j<P;j++){
		#pragma HLS PIPELINE II=1
						out_r[i*P+j]=BRAM_out_r[i][j];
				}
		}

}
}
