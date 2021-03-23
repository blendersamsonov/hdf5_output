#include <H5Cpp.h>

void initialize_file(const std::string & filepath) {
    auto file = H5::H5File(filepath, H5F_ACC_TRUNC);

    auto aplist = file.getAccessPlist();
    aplist.setCache(0, 1024, 1024*1024, 0.75);
    
    auto cplist = file.getCreatePlist();
    cplist.setFileSpaceStrategy(H5F_FSPACE_STRATEGY_PAGE, false, 1024);
    cplist.setFileSpacePagesize(1024*1024);
}

H5::DataSet create_dataset(const H5::Group & base_group, const std::string & name, const H5::DataSpace & dataspace, int chunks = 0) {
    H5::DSetCreatPropList prop;
    if (chunks > 0) {
        hsize_t chunk_dims[1] = {chunks};
        prop.setLayout(H5D_CHUNKED);
        prop.setChunk(1, chunk_dims);
    }
    return base_group.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace, prop);
}

void initialize_1d_extendable_dataset(const std::string & filepath, const std::string & name, int chunk_size) {
    H5::H5File file = H5::H5File(filepath, H5F_ACC_RDWR);

    hsize_t dims[1] = {0};
    hsize_t maxdims[1] = {H5S_UNLIMITED};
    H5::DataSpace dataspace(1, dims, maxdims);

    H5::DataSet dataset = create_dataset(file, name, dataspace, chunk_size);
}

void append_to_datasets(H5::DataSet *datasets[], int &entries, int ds_amount) {
    const int batch = 10000;
    int ds_size = 1 + entries / batch;
    hsize_t size[1] = {batch * ds_size};
    hsize_t offset[1] = {entries};
    hsize_t maxdims[1] = {H5S_UNLIMITED};    
    hsize_t mdims[1] = {1};
    H5::DataSet *dataset;
    H5::DataSpace write_dataspace(1, size, maxdims);
    H5::DataSpace memory_dataspace(1, mdims);
    write_dataspace.selectHyperslab(H5S_SELECT_SET, mdims, offset);
    
    for (int i = 0; i < ds_amount; i++) {
        dataset = datasets[i];
        if (!(entries % batch)) {
            dataset->extend(size);
        }

        double data[1] = {0.0};
        dataset->write(data, H5::PredType::NATIVE_DOUBLE, memory_dataspace, write_dataspace);
    }
    entries += 1;
}

int main(int argc, char * argv[]) {
    auto filename = std::string("test.h5");

    initialize_file(filename);

    const int ds_amount = 10;
    const int chunk_size = 1000;
    for (int i = 0; i < ds_amount; i++) {
        initialize_1d_extendable_dataset(filename, std::string("ds") + std::to_string(i), chunk_size);
    }

    const int amount = 1000000;

    H5::H5File file = H5::H5File(filename, H5F_ACC_RDWR);    
    H5::DataSet datasets[ds_amount];
    H5::DataSet * datasets_ptr[ds_amount];
    int entries = 0;


    for (int n = 0; n < ds_amount; n++) {
        datasets[n] = file.openDataSet(std::string("ds") + std::to_string(n));
        datasets_ptr[n] = &datasets[n];
    }

    for (int i = 0; i < amount; i++){
        append_to_datasets(datasets_ptr, entries, ds_amount);
    }
    
    return 1;
}