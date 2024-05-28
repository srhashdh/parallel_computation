/*
   solve 2-D discrete poisson equation with
   parallel Jacobi iteration
*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define nx 100
#define ny 100
#define npx 2
#define npy 5

int main(int argc, char *argv[])
{
    int myid, myidx, myidy, np, nlx, nly;
    int i, j, left, right, above, below, iter=0, itmax=8000;
    double a=1.0, b=1.0, hx=a/nx, hy=b/ny, d, dx, dy;
    double x0, y0, xi, yj;
    double t, rtmp, res=0.0, err=0.0, tol=1e-6;
    MPI_Status status;
    MPI_Datatype type1, type2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (npx*npy != np) {
        if ( myid == 0)
           printf("Error: npx*npy not equal to np!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

//  compute 2-D processor coordinates: myidx, myidy
    myidx = myid % npx;
    myidy = myid / npx;

//  compute number of local grid points: nlx, nly
    nlx = (nx-1) / npx + 1;
	if ( myidx < (nx-1) % npx )  nlx = nlx + 1;
	nly = (ny-1) / npy + 1;
	if ( myidy < (ny-1) % npy )  nly = nly + 1;
											
//  inital data
    double (*u)[nly+1], (*ut)[nly+1], (*unew)[nly+1], (*f)[nly];
    u    = (double*) calloc((nlx+1)*(nly+1), sizeof(double));
    ut   = (double*) calloc((nlx+1)*(nly+1), sizeof(double));
    unew = (double*) calloc((nlx+1)*(nly+1), sizeof(double));
    f    = (double*) calloc(nlx*nly, sizeof(double));

    if (!u | !ut | !f) {
        if ( myid == 0)
            printf("Out of memory!\n");
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

//  compute the coordinates of (0, 0) of the subdomain
    if ( myidx < (nx-1) % npx )  x0 = myidx * (nx-1)/npx + myidx;
	else  x0 = myidx * (nx-1)/npx + (nx-1) % npx;
    if ( myidy < (ny-1) % npy )  y0 = myidy * (ny-1)/npy + myidy;
	else  y0 = myidy * (ny-1)/npy + (ny-1) % npy;

    d  = 0.5*hx*hx*hy*hy / ( hx*hx + hy*hy );
    dx = d/(hx*hx);
    dy = d/(hy*hy);

//  true solution: ut(x,y) = - (x^2 + y^2)/4
    for (i=0; i<=nlx; i++) {
        for (j=0; j<=nly; j++) {
            xi = hx * (x0 + i);
            yj = hy * (y0 + j);
            ut[i][j] = -0.25*(xi*xi + yj*yj);
        }
    }

//  set initial guess and right hand side
    for (i=1; i<nlx; i++) {
        for (j=1; j<nly; j++) {
            u[i][j] = 0.0;
            f[i][j] = d;
        }
    }
 
//  boundary condition
    if (myidx == 0 )
        for (j=0; j<=nly; j++)
            u[0][j] = ut[0][j];
    if (myidx == npx - 1 )
        for (j=0; j<=nly; j++)
            u[nlx][j] = ut[nlx][j];
    if (myidy == 0 )
        for (i=0; i<=nlx; i++)
            u[i][0] = ut[i][0];
    if (myidy == npy - 1 )
        for (i=0; i<=nlx; i++)
            u[i][nly] = ut[i][nly];

    t = MPI_Wtime(); 

/***************************************************
    Jacobi iteration
/***************************************************/
//  compute left, right, above and below processes
    left = myidx - 1;  
    if ( left >= 0 )  left = myidy * npx + left;
    else left = MPI_PROC_NULL;

    right = myidx + 1;
    if ( right < npx )  right = myidy * npx + right;
    else  right = MPI_PROC_NULL;

    above = myidy + 1;
    if ( above < npy )  above = above * npx + myidx;
    else  above = MPI_PROC_NULL;

    below = myidy - 1;
    if ( below >= 0)  below = below * npx + myidx;
    else  below = MPI_PROC_NULL;


//  create two new mpi datatype
    MPI_Type_vector(nlx-1, 1, nly+1, MPI_DOUBLE, &type1);
    MPI_Type_contiguous(nly-1, MPI_DOUBLE, &type2);
    MPI_Type_commit(&type1);
    MPI_Type_commit(&type2);

//  begin iteration loop
    for (iter=1; iter<=itmax; iter++)
    {
        // Red points (i + j = 2k)
    	for (i = 1; i < nlx; i++)
    	{
        	for (j = 1; j < nly; j++)
        	{
            	if ((i + j) % 2 == 0)
            	{
                	unew[i][j] = f[i][j] + dx * (u[i + 1][j] + u[i - 1][j]) + dy * (u[i][j + 1] + u[i][j - 1]);
            	}
        	}
    	}

    	// Update red points
    	for (i = 1; i < nlx; i++)
    	{
        	for (j = 1; j < nly; j++)
        	{
            	if ((i + j) % 2 == 0)
            	{
                	u[i][j] = unew[i][j];
            	}
        	}
    	}

    	// Black points (i + j = 2k + 1)
    	for (i = 1; i < nlx; i++)
    	{
        	for (j = 1; j < nly; j++)
        	{
            	if ((i + j) % 2 != 0)
            	{
                	unew[i][j] = f[i][j] + dx * (u[i + 1][j] + u[i - 1][j]) + dy * (u[i][j + 1] + u[i][j - 1]);
            	}	
        	}
    	}

    	// Update black points
    	for (i = 1; i < nlx; i++)
    	{
        	for (j = 1; j < nly; j++)
        	{
            	if ((i + j) % 2 != 0)
            	{
                	u[i][j] = unew[i][j];
            	}	
        	}
    	}
// check residual
        res = 0.0;
        for (i=1; i<nlx; i++)
        for (j=1; j<nly; j++) {
            rtmp = f[i][j] - u[i][j]
                           + dx * (u[i+1][j] + u[i-1][j])
                           + dy * (u[i][j+1] + u[i][j-1]);
            if (res < fabs(rtmp))   res = fabs(rtmp);
        }
        rtmp = res / d;

        MPI_Allreduce(&rtmp, &res, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        if ( myid == 0 & iter % 1 == 0)
           printf("iter=%5d, residual=%.4e\n", iter, res);
		if ( res < tol) break;

//  update inner boundary values
//  u(1:nlx-1, 1) -> below, u(1:nlx-1, 0) <- below
        MPI_Sendrecv(&u[1][1], 1, type1, below, 111,
                     &u[1][0], 1, type1, below, 111, 
                     MPI_COMM_WORLD, &status);

//  u(1:nlx-1,nly-1) -> above, u(1:nlx-1,nly) <- above
        MPI_Sendrecv(&u[1][nly-1], 1, type1, above, 111,
                     &u[1][nly],   1, type1, above, 111,
                     MPI_COMM_WORLD, &status);

//  u(1,1:nly-1) -> left, u(0,1:nly-1) <- left
        MPI_Sendrecv(&u[1][1], 1, type2, left,  111,
                     &u[0][1], 1, type2, left,  111,
                     MPI_COMM_WORLD, &status);

//  u(nlx-1,1:nly-1) -> right, u(nlx,1:nly-1) <- right
        MPI_Sendrecv(&u[nlx-1][1], 1, type2, right, 111,
                     &u[nlx][1],   1, type2, right, 111,
                     MPI_COMM_WORLD, &status);
    }

    MPI_Type_free(&type1);
    MPI_Type_free(&type2);

    t = MPI_Wtime() - t; 
    if ( myid == 0)
        printf("Total iteration=%d, residual=%.4e, Time=%.4e\n", iter-1, res, t);

//  check solution
    err = 0.0;
    for (i=1; i<nlx; i++)
    for (j=1; j<nly; j++) {
         rtmp = fabs(ut[i][j] - u[i][j]);
         if (err < rtmp)  err = rtmp;
    }
	rtmp = err;

    MPI_Reduce(&rtmp, &err, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if ( myid == 0) 
        printf("||u-ut|| = %.4e\n", err);

    free(u); free(ut); free(unew); free(f);

    MPI_Finalize();
}


