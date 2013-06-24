#ifndef PVIZUTIL_H
#define PVIZUTIL_H

#include <math.h>
#include <string>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <H5Cpp.h>

using namespace std;
using namespace H5;

class PvizUtil
{
public:
	static void split(const int n, const int nsplit, int* counts, int* offsets)
	{
		if (nsplit == 0)
			throw "Cannot split by zero";
		
		// i = 0
		offsets[0] = 0;	
		counts[0] = ceil(n/(double)nsplit);
		
		// i > 0
		for (int i = 1; i < nsplit; i++)
		{
			offsets[i] = offsets[i-1] + counts[i-1];
			counts[i] = ceil((n - offsets[i])/(double)(nsplit - i));
		}
	};
	
	static bool H5CheckDataset(const std::string &filename, 
							   const std::string &datasetname)
	{
		bool rtn = true;
		try {
			H5::H5File file(filename, H5F_ACC_RDONLY );
			H5::DataSet dataset = file.openDataSet(datasetname);
		}
		catch (H5::FileIException e) 
		{
			rtn = false;
		}
		
		return rtn;
	};
	
	static void H5GetDatasetDim(const std::string &filename, 
								const std::string &datasetname,
								const int &rank, 
								hsize_t *dim)
	{
		H5::H5File file( filename, H5F_ACC_RDONLY );
		H5::DataSet dataset = file.openDataSet(datasetname);
		
		/*
		 * Get dataspace of the dataset.
		 */
		H5::DataSpace dataspace = dataset.getSpace();
		
		/*
		 * Get the number of dimensions in the dataspace.
		 */
		if (rank != dataspace.getSimpleExtentNdims())
		{
			throw std::invalid_argument("Rank mismatch!!");
		}
		
		dataspace.getSimpleExtentDims(dim, NULL);
	};
	
	static void H5ReadByOffset(const std::string &filename, 
							   const std::string &datasetname, 
							   const H5::DataType &mem_type,
							   const size_t &rowOffset, 
							   const size_t &size,
							   void *m)
	{
		H5::H5File file(filename, H5F_ACC_RDONLY);
		H5::DataSet dataset = file.openDataSet(datasetname);
		H5::DataSpace dataspace = dataset.getSpace();
		
		checkDataset(dataset, mem_type, rowOffset, size, m);
		
		hsize_t      offset[1];   // hyperslab offset in the file
		hsize_t      count[1];    // size of the hyperslab in the file
		offset[0] = rowOffset;
		count[0]  = size;
		
		dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);
		
		H5::DataSpace memspace(1, count);
		
		dataset.read(m, mem_type, memspace, dataspace);
	};
	
	static void H5ReadByRow(const std::string &filename, 
							const std::string &datasetname, 
							const H5::DataType &mem_type,
							const size_t &rowOffset, 
							const size_t &size1,
							const size_t &size2,
							void *m)
	{
		H5::H5File file(filename, H5F_ACC_RDONLY);
		H5::DataSet dataset = file.openDataSet(datasetname);
		H5::DataSpace dataspace = dataset.getSpace();
		
		checkDataset(dataset, mem_type, rowOffset, size1, size2, m);
		
		hsize_t      offset[2];   // hyperslab offset in the file
		hsize_t      count[2];    // size of the hyperslab in the file
		offset[0] = rowOffset;
		offset[1] = 0;
		count[0]  = size1;
		count[1]  = size2;
		
		dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);
		
		H5::DataSpace memspace(2, count);
		
		dataset.read(m, mem_type, memspace, dataspace);
	};
	
	static void H5WriteByRow(const std::string &filename, 
							 const std::string &datasetname, 
							 const H5::DataType &mem_type,
							 const size_t &rowOffset, 
							 const size_t &size1,
							 const size_t &size2,
							 const gsl_matrix *m)
	{
		H5::H5File file(filename, H5F_ACC_RDWR);
		H5::DataSet dataset = file.openDataSet(datasetname);
		H5::DataSpace dataspace = dataset.getSpace();
		
		checkDataset(dataset, mem_type, rowOffset, size1, size2, m);
		
		hsize_t      offset[2];   // hyperslab offset in the file
		hsize_t      count[2];    // size of the hyperslab in the file
		offset[0] = rowOffset;
		offset[1] = 0;
		count[0]  = size1;
		count[1]  = size2;
		
		dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);
		
		H5::DataSpace memspace(2, count);
		
		dataset.write(m, mem_type, memspace, dataspace);
	};
	
protected:
	static void checkDataset(const H5::DataSet& dataset, 
							 const H5::DataType &mem_type,
							 const size_t rowOffset, 
							 const size_t &size1,
							 const size_t &size2,
							 const void *m)
	{
		// Check datatype
		/*
		H5T_class_t class_t = dataset.getTypeClass();
		
		if ((class_t != H5T_FLOAT) && (class_t != H5T_INTEGER))
		{
			throw std::runtime_error("Type mismatch!!");
		}			
		*/
		
		H5::DataType dtype = dataset.getDataType();
		if (!(dtype == mem_type))
		{
			throw std::runtime_error("Data type mismatch!!");
		}
		
		H5::DataSpace dataspace = dataset.getSpace();
		
		// Check rank
		if (dataspace.getSimpleExtentNdims() != 2)
		{
			throw std::runtime_error("Rank mismatch!!");
		}
		
		// Check dimension
		hsize_t dims_out[2];
		dataspace.getSimpleExtentDims(dims_out, NULL);
		if (dims_out[0] < size1)
		{
			throw std::runtime_error("Dimension mismatch!!");
		}
		
		if (dims_out[1] != size2)
		{
			throw std::runtime_error("Dimension mismatch!!");
		}
		
		// Check parameter
		if (dims_out[0] <= rowOffset)
		{
			throw std::runtime_error("Offset is out of range!!");
		}
		
		if (dims_out[0] < rowOffset + size1)
		{
			throw std::runtime_error("Count is out of range!!");
		}
	};
	
	static void checkDataset(const H5::DataSet& dataset, 
							 const H5::DataType &mem_type,
							 const size_t rowOffset, 
							 const size_t &size,
							 const void *m)
	{
		// Check datatype
		/*
		 H5T_class_t class_t = dataset.getTypeClass();
		 
		 if ((class_t != H5T_FLOAT) && (class_t != H5T_INTEGER))
		 {
		 throw std::runtime_error("Type mismatch!!");
		 }			
		 */
		
		H5::DataType dtype = dataset.getDataType();
		if (!(dtype == mem_type))
		{
			throw std::runtime_error("Data type mismatch!!");
		}
		
		H5::DataSpace dataspace = dataset.getSpace();
		
		// Check rank
		if (dataspace.getSimpleExtentNdims() != 1)
		{
			throw std::runtime_error("Rank mismatch!!");
		}
		
		// Check dimension
		hsize_t dims_out[1];
		dataspace.getSimpleExtentDims(dims_out, NULL);
		if (dims_out[0] < size)
		{
			throw std::runtime_error("Dimension mismatch!!");
		}
		
		// Check parameter
		if (dims_out[0] <= rowOffset)
		{
			throw std::runtime_error("Offset is out of range!!");
		}
		
		if (dims_out[0] < rowOffset + size)
		{
			throw std::runtime_error("Count is out of range!!");
		}
	};
};

#endif //PVIZUTIL_H
