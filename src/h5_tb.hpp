#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <functional>
#include <exception>
#include <stdexcept>

#include "file.h"
#include "group.h"
#include "H5TBpublic.h"
#include "H5Dpublic.h"
#include "H5Tpublic.h"

namespace NodeHDF5 {

    class H5tb {
    public:
        static void Initialize (Handle<Object> target) {

            // append this function to the target object
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::make_table)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_table)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "appendRecords"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::append_records)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeRecords"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_records)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readRecords"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_records)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "deleteRecord"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::delete_record)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "insertRecord"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::insert_record)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeFieldsName"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_fields_name)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeFieldsIndex"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_fields_index)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readFieldsName"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_fields_name)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readFieldsIndex"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_fields_index)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getTableInfo"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::get_table_info)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getFieldInfo"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::get_field_info)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "deleteField"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::delete_field)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "insertField"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::insert_field)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "addRecordsFrom"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::add_records_from)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "combineTables"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::combine_tables)->GetFunction());
        }

        static std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>> prepareData(Local<v8::Array>& table){
            hsize_t nrecords=0;
            size_t type_size=0;
            std::unique_ptr<size_t[]> field_offsets(new size_t[table->Length()]);
            std::unique_ptr<size_t[]> field_sizes(new size_t[table->Length()]);
            std::unique_ptr<hid_t[]> field_types(new hid_t[table->Length()]);
            for (uint32_t i = 0; i < table->Length(); i++)
            {
                if(table->Get(i)->IsFloat64Array())
                {
                    Local<v8::Float64Array> field=Local<v8::Float64Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=8;
                    type_size+=8;
                    field_types[i]=H5T_NATIVE_DOUBLE;
                }
                else if(table->Get(i)->IsUint32Array())
                {
                    Local<v8::Uint32Array> field=Local<v8::Uint32Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=4;
                    type_size+=4;
                    field_types[i]=H5T_NATIVE_UINT;
                }
                else if(table->Get(i)->IsInt32Array())
                {
                    Local<v8::Int32Array> field=Local<v8::Int32Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=4;
                    type_size+=4;
                    field_types[i]=H5T_NATIVE_INT;
                }
                else if(table->Get(i)->IsUint16Array())
                {
                    Local<v8::Uint16Array> field=Local<v8::Uint16Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=2;
                    type_size+=2;
                    field_types[i]=H5T_NATIVE_USHORT;
                }
                else if(table->Get(i)->IsInt16Array())
                {
                    Local<v8::Int16Array> field=Local<v8::Int16Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=2;
                    type_size+=2;
                    field_types[i]=H5T_NATIVE_SHORT;
                }
                else if(table->Get(i)->IsUint8Array())
                {
                    Local<v8::Uint8Array> field=Local<v8::Uint8Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=1;
                    type_size+=1;
                    field_types[i]=H5T_NATIVE_UINT8;
                }
                else if(table->Get(i)->IsInt8Array())
                {
                    Local<v8::Int8Array> field=Local<v8::Int8Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    field_sizes[i]=1;
                    type_size+=1;
                    field_types[i]=H5T_NATIVE_INT8;
                }
                else if(table->Get(i)->IsArray())
                {
                    Local<v8::Array> field=Local<v8::Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    size_t max=0;
                    for(uint32_t j=0;j<nrecords;j++){
                        size_t len=field->Get(j)->ToString()->Length ();
                        if(max<len)max=len;
                    }
                    field_sizes[i]=max;
                    type_size+=max;
                    hid_t string_type = H5Tcopy(H5T_C_S1);
                    H5Tset_size(string_type, max);
                    field_types[i]=string_type;
                }
                else
                {
                    throw std::invalid_argument("unsupported data type");
                }
            }
//            std::cout<<"type_size "<<type_size<<" "<<nrecords<<std::endl;
            std::unique_ptr<char[]> data(new char[type_size*nrecords]);
            for (uint32_t i = 0; i < table->Length(); i++)
            {
                if(table->Get(i)->IsFloat64Array())
                {
                    Local<v8::Float64Array> field=Local<v8::Float64Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((double*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToNumber()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsUint32Array())
                {
                    Local<v8::Uint32Array> field=Local<v8::Uint32Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToUint32()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsInt32Array())
                {
                    Local<v8::Int32Array> field=Local<v8::Int32Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInt32()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsUint16Array())
                {
                    Local<v8::Uint16Array> field=Local<v8::Uint16Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned short*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInteger()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsInt16Array())
                {
                    Local<v8::Int16Array> field=Local<v8::Int16Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned short*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInteger()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsUint8Array())
                {
                    Local<v8::Uint8Array> field=Local<v8::Uint8Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned char*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInteger()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsInt8Array())
                {
                    Local<v8::Int8Array> field=Local<v8::Int8Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned char*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInteger()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsArray())
                {
                    Local<v8::Array> field=Local<v8::Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        String::Utf8Value value (field->Get(j)->ToString());
                        std::memcpy(&data[j*type_size+field_offsets[i]], (*value), H5Tget_size(field_types[i]));
                    }
                    
                }
                
            }
            
            return std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>(nrecords, type_size, std::move(field_offsets), std::move(field_sizes), std::move(field_types), std::move(data));
        }
        
        static void prepareTable(hsize_t nrecords, hsize_t nfields, std::unique_ptr<int[]> field_indices, size_t type_size, hid_t& dataset, hid_t& dataset_type, char* field_names[], std::unique_ptr<size_t[]> field_offsets, std::unique_ptr<char[]> data, Local<v8::Array>& table){
                for (uint32_t i = 0; i < nfields; i++)
                {
                    hid_t type=H5Tget_member_type(dataset_type, field_indices[i]);
//                    std::cout<<" "<<field_names[i]<<" "<<H5Tget_class(type)<<std::endl;
                    switch(H5Tget_class(type)){
                        case H5T_FLOAT:
                            if(H5Tget_precision(type)==64)
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 8*nrecords);
                                Local<Float64Array> buffer = Float64Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((double*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            else if(H5Tget_precision(type)==32)
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                Local<Float32Array> buffer = Float32Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((double*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            break;
                        case H5T_INTEGER:
                            switch(H5Tget_precision(type))
                            {
                                case 32:
                                if(H5Tget_sign(type)==H5T_SGN_NONE)
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                    Local<Uint32Array> buffer = Uint32Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                else
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                    Local<Int32Array> buffer = Int32Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((int*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                break;
                                case 16:
                                if(H5Tget_sign(type)==H5T_SGN_NONE)
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 2*nrecords);
                                    Local<Uint16Array> buffer = Uint16Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((unsigned short*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                else
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 2*nrecords);
                                    Local<Int16Array> buffer = Int16Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((short*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                break;
                                case 8:
                                if(H5Tget_sign(type)==H5T_SGN_NONE)
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), nrecords);
                                    Local<Uint8Array> buffer = Uint8Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((unsigned char*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                else
                                {
                                    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), nrecords);
                                    Local<Int8Array> buffer = Int8Array::New(arrayBuffer, 0, nrecords);
                                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                    for(uint32_t j=0;j<nrecords;j++){
                                        buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((char*)&data[j*type_size+field_offsets[i]])[0]));
                                    }

                                    table->Set(i, buffer);
                                }
                                break;
                            }
                            break;
//                        case H5T_CSET_ASCII:
                        case H5T_STRING:
//                            if(H5Tget_sign(type)==H5T_SGN_NONE)
                            {
                                Local<Array> buffer = Array::New(v8::Isolate::GetCurrent(), nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    std::string cell(&data[j*type_size+field_offsets[i]], H5Tget_size(type));
                                    buffer->Set(j, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), cell.c_str()));
                                }

                                table->Set(i, buffer);
                            }
                            break;
                        default:
                            throw std::invalid_argument("unsupported data type");
                            break;
                    }
                    H5Tclose(type);
                }
            
        }
        
        static void make_table (const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 3 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[2]);
            char* field_names[table->Length()];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < table->Length(); i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
                String::Utf8Value field_name (table->Get(i)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(),"name"))->ToString());
                std::string fieldName((*field_name)); 
                std::memcpy(field_names[i], fieldName.c_str(), fieldName.length());
            }
            try{
                std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
                hsize_t chunk_size = 10;
                int *fill_data = NULL;
                herr_t err=H5TBmake_table( (*table_name), args[0]->ToInt32()->Value(), (*table_name), table->Length(), std::get<0>(data), std::get<1>(data), (const char**)field_names, std::get<2>(data).get(), std::get<4>(data).get(), chunk_size, fill_data, 0, (const void*)std::get<5>(data).get());
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed making table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
//            /* release */
            for (unsigned int i = 0; i < table->Length(); i++)
            {
                free ( field_names[i] );
            }
        }
        
        static void read_table (const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsString()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
//            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed getting field info , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            size_t dst_size=0;
            std::unique_ptr<int[]> field_indices(new int[nfields]);
            for (unsigned int i = 0; i < nfields; i++)
            {
                dst_size+=field_size[i];
                field_indices[i]=i;
//                std::cout<<" "<<field_names[i]<<" "<<field_size[i]<<" "<<field_offsets[i]<<std::endl;

            }
//            std::cout<<dst_size<<" "<<type_size<<" nrecords "<<nrecords<<std::endl;
            std::unique_ptr<char[]> data(new char[type_size*nrecords]);
            err=H5TBread_table (args[0]->ToInt32()->Value(), (*table_name), type_size,  field_offsets.get(), field_size.get(),  (void*) data.get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed reading table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                hid_t dataset_type=H5Dget_type(dataset );
                v8::Local< v8::Array > 	table=v8::Array::New (v8::Isolate::GetCurrent(), nfields);
                try{
                prepareTable(nrecords, nfields, std::move(field_indices), type_size, dataset, dataset_type, field_names, std::move(field_offsets), std::move(data), table);
                }
                catch(std::exception& ex){
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                            args.GetReturnValue().SetUndefined();
                            return;
                }
                H5Tclose(dataset_type);
                H5Dclose(dataset);
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
                args.GetReturnValue().Set(table);
            
        }
        
        static void append_records(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 3 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[2]);
            std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
            try{
                herr_t err=H5TBappend_records (args[0]->ToInt32()->Value(), (*table_name), std::get<0>(data), std::get<1>(data), std::get<2>(data).get(), std::get<3>(data).get(), (const void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed appending to table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
        }
        
        static void write_records(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[3]);
            std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
            try{
                herr_t err=H5TBwrite_records (args[0]->ToInt32()->Value(), (*table_name), args[2]->ToInt32()->Value(), std::get<0>(data), std::get<1>(data), std::get<2>(data).get(), std::get<3>(data).get(), (const void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed writing to table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
        }
        
        static void read_records(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
//            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            std::unique_ptr<int[]> field_indices(new int[nfields]);
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_indices[i]=i;
//                std::cout<<" "<<field_names[i]<<" "<<field_size[i]<<" "<<field_offsets[i]<<std::endl;

            }

            std::unique_ptr<char[]> data(new char[type_size*args[3]->ToInt32()->Value()]);
            err=H5TBread_records (args[0]->ToInt32()->Value(), (*table_name), args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value(), type_size,  field_offsets.get(), field_size.get(),  (void*) data.get() );
                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                hid_t dataset_type=H5Dget_type(dataset );
                v8::Local< v8::Array > 	table=v8::Array::New (v8::Isolate::GetCurrent(), nfields);
                try{
                prepareTable(args[3]->ToInt32()->Value(), nfields, std::move(field_indices), type_size, dataset, dataset_type, field_names, std::move(field_offsets), std::move(data), table);
                }
                catch(std::exception& ex){
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                            args.GetReturnValue().SetUndefined();
                            return;
                }
                H5Tclose(dataset_type);
                H5Dclose(dataset);
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
                args.GetReturnValue().Set(table);
            
        }
        
        static void delete_record(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            herr_t err = H5TBdelete_record(args[0]->ToInt32()->Value(), (*table_name), args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value());
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed deleting records from table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        
        static void insert_record(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[3]);
            std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
            try{
                herr_t err=H5TBinsert_record (args[0]->ToInt32()->Value(), (*table_name), args[2]->ToInt32()->Value(), std::get<0>(data), std::get<1>(data), std::get<2>(data).get(), std::get<3>(data).get(), (void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed inserting to table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
        }
        
        static void write_fields_name(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[3]);
            char* model_field_names[table->Length()];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            std::string fieldNames="";
            for (unsigned int i = 0; i < table->Length(); i++)
            {
                model_field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( model_field_names[i], 0, 255 );
                String::Utf8Value field_name (table->Get(i)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(),"name"))->ToString());
                std::string fieldName((*field_name));
                fieldNames+=fieldName;
                std::memcpy(model_field_names[i], fieldName.c_str(), fieldName.length());
                if(i<table->Length()-1)fieldNames+=",";
            }
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            try{
                std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
                
                herr_t err=H5TBwrite_fields_name (args[0]->ToInt32()->Value(), (*table_name), (const char*)fieldNames.c_str(), args[2]->ToInt32()->Value(), std::get<0>(data), std::get<1>(data), std::get<2>(data).get(), std::get<3>(data).get(), (const void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed overwriting fields in table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
            args.GetReturnValue().SetUndefined();
            return;
        }
        
        static void write_fields_index(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 5 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray() || !args[4]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model, indices")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[3]);
            Local<v8::Array> indices=Local<v8::Array>::Cast(args[4]);
            std::unique_ptr<int[]> field_indices(new int[indices->Length()]);
            for (unsigned int i = 0; i < indices->Length(); i++)
            {
                field_indices[i]=indices->Get(i)->ToUint32()->Value();
            }
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            try{
                std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
                
                herr_t err=H5TBwrite_fields_index (args[0]->ToInt32()->Value(), (*table_name), table->Length(), field_indices.get(), args[2]->ToInt32()->Value(), std::get<0>(data), std::get<1>(data), std::get<2>(data).get(), std::get<3>(data).get(), (const void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed overwriting fields in table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
            args.GetReturnValue().SetUndefined();
            return;
        }
        
        static void read_fields_name(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 5 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() || !args[4]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords, model, field names")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            Local<v8::Array> indices=Local<v8::Array>::Cast(args[4]);
            std::string fieldNames="";
            char* model_field_names[indices->Length()];
            for (unsigned int i = 0; i < indices->Length(); i++)
            {
                String::Utf8Value field_name (indices->Get(i)->ToString());
                std::string fieldName((*field_name));
                model_field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( model_field_names[i], 0, 255 );
                std::strcpy(model_field_names[i],fieldName.c_str());
                fieldNames.append(fieldName);
                if(i<indices->Length()-1)fieldNames.append(",");
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
//            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            std::unique_ptr<size_t[]> model_field_size(new size_t[indices->Length()]);
            std::unique_ptr<size_t[]> model_field_offsets(new size_t[indices->Length()]);
            size_t model_type_size=0;
            size_t model_size=0;
                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                hid_t dataset_type=H5Dget_type(dataset );
            std::unique_ptr<int[]> field_indices(new int[indices->Length()]);
            for (unsigned int j = 0; j < indices->Length(); j++)
            {
                size_t model_type_offset=0;
                bool hit=false;
                String::Utf8Value field_name (indices->Get(j)->ToString());
                std::string fieldName((*field_name));
                for (unsigned int i = 0; !hit && i < nfields; i++)
                {
//                    std::cout<<" "<<field_names[i]<<" "<<field_size[i]<<" "<<field_offsets[i]<<std::endl;
                        hid_t type=H5Tget_member_type(dataset_type, i);
                    if(fieldName.compare(field_names[i])==0){
                        model_field_offsets[j]=model_type_size;
                        model_field_size[j]=H5Tget_size(type);
                        model_size+=field_size[i];
                        model_type_size+=H5Tget_size(type);
                        model_type_offset+=H5Tget_size(type);
                        field_indices[j]=i;
                         hit=true;
                    }
                        H5Tclose(type);
                }
            }
                H5Tclose(dataset_type);
                H5Dclose(dataset);

                std::unique_ptr<char[]> data(new char[model_type_size*args[3]->ToInt32()->Value()]);
                err=H5TBread_fields_name (args[0]->ToInt32()->Value(), (*table_name), fieldNames.c_str(), args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value(), model_type_size,  model_field_offsets.get(), model_field_size.get(),  (void*) data.get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed reading fields in table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                v8::Local< v8::Array > 	table=v8::Array::New (v8::Isolate::GetCurrent(), indices->Length());
                dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                dataset_type=H5Dget_type(dataset );
                try{
                    prepareTable(args[3]->ToInt32()->Value(), indices->Length(), std::move(field_indices), model_type_size, dataset, dataset_type, model_field_names, std::move(model_field_offsets), std::move(data), table);
                }
                catch(std::exception& ex){
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                            args.GetReturnValue().SetUndefined();
                            return;
                }
                H5Tclose(dataset_type);
                H5Dclose(dataset);
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
                args.GetReturnValue().Set(table);
        }
        
        static void read_fields_index(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 5 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() || !args[4]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords, model, field indices")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            Local<v8::Array> indices=Local<v8::Array>::Cast(args[4]);
            std::string fieldNames="";
            char* model_field_names[indices->Length()];
            std::unique_ptr<int[]> field_indices(new int[indices->Length()]);
            for (unsigned int i = 0; i < indices->Length(); i++)
            {
                field_indices[i]=indices->Get(i)->ToUint32()->Value();
                model_field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( model_field_names[i], 0, 255 );
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
//            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            std::unique_ptr<size_t[]> model_field_size(new size_t[indices->Length()]);
            std::unique_ptr<size_t[]> model_field_offsets(new size_t[indices->Length()]);
            size_t model_type_size=0;
            size_t model_size=0;
                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                hid_t dataset_type=H5Dget_type(dataset );
            for (unsigned int j = 0; j < indices->Length(); j++)
            {
                size_t model_type_offset=0;
                bool hit=false;
                for (unsigned int i = 0; !hit && i < nfields; i++)
                {
//                    std::cout<<" "<<field_names[i]<<" "<<field_size[i]<<" "<<field_offsets[i]<<std::endl;
                        hid_t type=H5Tget_member_type(dataset_type, i);
                    if(field_indices[j]==i){
                        std::string fieldName(field_names[i]);
                        fieldName+=fieldName;
                        if(j < indices->Length()-1)fieldName.append(",");
                        std::strcpy( model_field_names[j], field_names[i]);
                        model_field_offsets[j]=model_type_size;
                        model_field_size[j]=H5Tget_size(type);
//                    std::cout<<"hit "<<model_size<<" "<<model_type_offset<<" "<<H5Tget_size(type)<<std::endl;
                        model_size+=field_size[i];
                        model_type_size+=H5Tget_size(type);
                        model_type_offset+=H5Tget_size(type);
                         hit=true;
                    }
                        H5Tclose(type);
                }
            }
                H5Tclose(dataset_type);
                H5Dclose(dataset);

                std::unique_ptr<char[]> data(new char[model_type_size*args[3]->ToInt32()->Value()]);
                err=H5TBread_fields_index (args[0]->ToInt32()->Value(), (*table_name), indices->Length(), field_indices.get(), args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value(), model_type_size,  model_field_offsets.get(), model_field_size.get(),  (void*) data.get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed reading fields in table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                v8::Local< v8::Array > 	table=v8::Array::New (v8::Isolate::GetCurrent(), indices->Length());
                dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                dataset_type=H5Dget_type(dataset );
                try{
                    prepareTable(args[3]->ToInt32()->Value(), indices->Length(), std::move(field_indices), model_type_size, dataset, dataset_type, model_field_names, std::move(model_field_offsets), std::move(data), table);
                }
                catch(std::exception& ex){
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                            args.GetReturnValue().SetUndefined();
                            return;
                }
                H5Tclose(dataset_type);
                H5Dclose(dataset);
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
                args.GetReturnValue().Set(table);
        }
        
        static void delete_field(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 3 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsString()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, field name")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            String::Utf8Value field_name (args[2]->ToString());
            herr_t err = H5TBdelete_field(args[0]->ToInt32()->Value(), (*table_name), (*field_name));
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed deleting field from table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        
        static void add_records_from(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 6 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() || !args[4]->IsString() || !args[5]->IsUint32()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name1, start1, nrecords, name2, start2")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name1 (args[1]->ToString());
            String::Utf8Value table_name2 (args[4]->ToString());
            std::cout<<args[0]->ToInt32()->Value()<<" "<<(*table_name1)<<" "<<args[2]->ToInt32()->Value()<<" "<<args[3]->ToInt32()->Value()<<" "<<(*table_name2)<<" "<<args[5]->ToInt32()->Value()<<std::endl;
            herr_t err=H5TBadd_records_from (args[0]->ToInt32()->Value(), (*table_name1),args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value(), (*table_name2), args[5]->ToInt32()->Value());
            if (err < 0) {
                std::string tableName(*table_name1);
                std::string errStr="Failed add records from table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        
        static void combine_tables(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 5 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsString() || !args[4]->IsString()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id1, name1, id2, name2, name3")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name1 (args[1]->ToString());
            String::Utf8Value table_name2 (args[3]->ToString());
            String::Utf8Value table_name3 (args[4]->ToString());
            herr_t err=H5TBcombine_tables (args[0]->ToInt32()->Value(), (*table_name1),args[2]->ToInt32()->Value(), (*table_name2), (*table_name3));
            if (err < 0) {
                std::string tableName(*table_name1);
                std::string errStr="Failed combining from table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        
        static void insert_field(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 4 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[3]);
            String::Utf8Value field_name (table->Get(0)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(),"name"))->ToString());
            std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&& data=prepareData(table);
            try{
                int *fill_data = NULL;
                herr_t err=H5TBinsert_field (args[0]->ToInt32()->Value(), (*table_name), (*field_name), std::get<4>(data)[0], args[2]->ToInt32()->Value(), fill_data, (void*)std::get<5>(data).get() );
                if (err < 0) {
                    std::string tableName(*table_name);
                    std::string errStr="Failed inserting to table, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            catch(std::exception& ex){
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
                    args.GetReturnValue().SetUndefined();
                    return;
                
            }
        }
        
        static void get_table_info(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsString()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            v8::Local<v8::Object> obj = v8::Object::New(v8::Isolate::GetCurrent());
            obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "nfields"),  Uint32::New(v8::Isolate::GetCurrent(), nfields));
            obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "nrecords"),  Uint32::New(v8::Isolate::GetCurrent(), nrecords));
            args.GetReturnValue().Set(obj);
        }
        
        static void get_field_info(const v8::FunctionCallbackInfo<Value>& args)
        {
            // fail out if arguments are not correct
            if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsString()) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
                args.GetReturnValue().SetUndefined();
                return;

            }
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
//            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            herr_t err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), nfields);
            for (unsigned int i = 0; i < nfields; i++)
            {
                array->Set(i, String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                free ( field_names[i] );
            }
            args.GetReturnValue().Set(array);
        }
        
    };
}