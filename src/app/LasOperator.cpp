#include "LasOperator.h"

#include<string>
#include<chrono>
#include <ctime>


LasHeader LasOperate::pointReadHeader() {
	LasHeader header;
	laszip_POINTER lasreader;
	laszip_create(&lasreader);

	std::string local = path;
	laszip_BOOL compressed;
	laszip_open_reader(lasreader, local.c_str(), &compressed);

	laszip_header* lasheader;
	laszip_get_header_pointer(lasreader, &lasheader);

	header.num_points =
		std::max((uint32_t)lasheader->number_of_point_records, (uint32_t)lasheader->extended_number_of_point_records);
	header.offset.x() = lasheader->x_offset;
	header.offset.y() = lasheader->y_offset;
	header.offset.z() = lasheader->z_offset;



	laszip_close_reader(lasreader);
	laszip_destroy(lasreader);
	return header;
}

CloudStruct LasOperate::pointRead()
{
	laszip_POINTER lasreader;
	laszip_create(&lasreader);

	string local = path;
	laszip_BOOL compressed;
	laszip_open_reader(lasreader, local.c_str(), &compressed);

	laszip_header* lasheader;
	laszip_get_header_pointer(lasreader, &lasheader);
	laszip_I64 num_points = (lasheader->number_of_point_records ? lasheader->number_of_point_records
		: lasheader->extended_number_of_point_records);
	CloudStruct myCloud;
	myCloud.ptsxyz.points.resize(num_points);
	laszip_point* point;
	laszip_get_point_pointer(lasreader, &point);

	for (int i = 0; i < num_points; i++) {
		if (laszip_read_point(lasreader))
			cout << "read point error!" << endl;
		myCloud.ptsxyz[i].x = (double)point->X * lasheader->x_scale_factor + lasheader->x_offset;

		myCloud.ptsxyz[i].y = (double)point->Y * lasheader->y_scale_factor + lasheader->y_offset;
		myCloud.ptsxyz[i].z = (double)point->Z * lasheader->z_scale_factor + lasheader->z_offset;
		//myCloud.classifcation[i] = 1;
		myCloud.classifcation.push_back((int)point->classification);
	}

	laszip_close_reader(lasreader);
	laszip_destroy(lasreader);

	return myCloud;
}



void LasOperate::MyPcSave(const std::string& outpath, const CloudStruct& outCloud)
{
	laszip_POINTER lasreader;
	laszip_create(&lasreader);
	string readPath = path;
	laszip_BOOL compressed;
	laszip_open_reader(lasreader, readPath.c_str(), &compressed);

	laszip_header* header_read;
	laszip_get_header_pointer(lasreader, &header_read);


	laszip_POINTER writer;
	laszip_create(&writer);


	laszip_BOOL out_compressed = (strstr(outpath.c_str(), "laz") != 0);

	laszip_header* header_write;
	laszip_get_header_pointer(writer, &header_write);

	//??????????
	header_write->file_source_ID = header_read->file_source_ID;
	header_write->global_encoding = header_read->global_encoding; // see LAS specification for details
	header_write->version_major = header_read->version_major;
	header_write->version_minor = header_read->version_minor;
	strncpy(header_write->system_identifier, "LInSAR", 7);
	header_write->header_size = header_read->header_size;//375;
	header_write->offset_to_point_data = header_read->offset_to_point_data;
	header_write->point_data_format = header_read->point_data_format;
	header_write->point_data_record_length = header_read->point_data_record_length;
	header_write->number_of_point_records = outCloud.ptsxyz.points.size();
	header_write->number_of_points_by_return[0] = outCloud.ptsxyz.points.size();
	header_write->extended_number_of_point_records = outCloud.ptsxyz.points.size();
	header_write->extended_number_of_points_by_return[0] = outCloud.ptsxyz.points.size();
	header_write->x_scale_factor = 0.1;
	header_write->y_scale_factor = 0.1;
	header_write->z_scale_factor = 0.1;
	for (int i = 0; i < 5; i++)
	{
		header_write->number_of_points_by_return[i] = header_read->number_of_points_by_return[i];
	}
	header_write->x_offset = header_read->x_offset;
	header_write->y_offset = header_read->y_offset;
	header_write->z_offset = header_read->z_offset;
	header_write->max_x = header_read->max_x;
	header_write->min_x = header_read->min_x;
	header_write->max_y = header_read->max_y;
	header_write->min_y = header_read->min_y;
	header_write->max_z = header_read->max_z;
	header_write->min_z = header_read->min_z;


	// LAS 1.3 and higher only
	header_write->start_of_waveform_data_packet_record = header_read->start_of_waveform_data_packet_record;

	// LAS 1.4 and higher only
	header_write->start_of_first_extended_variable_length_record = header_read->start_of_first_extended_variable_length_record;
	header_write->number_of_extended_variable_length_records = header_read->number_of_extended_variable_length_records;
	header_write->extended_number_of_point_records = outCloud.ptsxyz.points.size(); //header_read->extended_number_of_point_records;
	for (int i = 0; i < 15; i++)
	{
		header_write->extended_number_of_points_by_return[i] = header_read->extended_number_of_points_by_return[i];
	}

	// we may modify output because we omit any user defined data that may be ** the header

	if (header_read->user_data_in_header_size)
	{
		header_write->header_size -= header_read->user_data_in_header_size;
		header_write->offset_to_point_data -= header_read->user_data_in_header_size;
		fprintf(stderr, "omitting %d bytes of user_data_in_header\n", header_read->user_data_after_header_size);
	}

	//??????????

	laszip_BOOL request = 1;
	if (laszip_request_compatibility_mode(writer, request)) {
		char* err;
		laszip_get_error(writer, &err);
		std::cout << err;
	}

	if (laszip_open_writer(writer, outpath.c_str(), out_compressed)) {
		char* err;
		laszip_get_error(writer, &err);
		std::cout << err;
	};


	laszip_point* point_read;
	if (laszip_get_point_pointer(lasreader, &point_read)) {
		char* err;
		laszip_get_error(lasreader, &err);
		std::cout << err;
	}

	laszip_point* point_write;
	if (laszip_get_point_pointer(writer, &point_write)) {
		char* err;
		laszip_get_error(writer, &err);
		std::cout << err;
	}
	int count = 0;
	std::cout <<"cpp????"<< outCloud.ptsxyz.points.size() << endl;
	
	for (int i = 0; i < outCloud.ptsxyz.points.size(); i++) {
		count++;
		laszip_read_point(lasreader);

		point_write->X = point_read->X;
		point_write->Y = point_read->Y;
		point_write->Z = point_read->Z;
		point_write->intensity = point_read->intensity;
		point_write->return_number = point_read->return_number;
		point_write->number_of_returns = point_read->number_of_returns;
		point_write->scan_direction_flag = point_read->scan_direction_flag;
		point_write->edge_of_flight_line = point_read->edge_of_flight_line;
		point_write->classification = outCloud.classifcation[i];

		point_write->withheld_flag = point_read->withheld_flag;
		point_write->keypoint_flag = point_read->keypoint_flag;
		point_write->synthetic_flag = point_read->synthetic_flag;
		point_write->scan_angle_rank = point_read->scan_angle_rank;
		point_write->user_data = point_read->user_data;
		point_write->point_source_ID = point_read->point_source_ID;

		point_write->gps_time = point_read->gps_time;
		memcpy(point_write->rgb, point_read->rgb, 8);
		memcpy(point_write->wave_packet, point_read->wave_packet, 29);
		// LAS 1.4 only
		point_write->extended_scanner_channel = point_read->extended_scanner_channel;
		point_write->extended_classification_flags = point_read->extended_classification_flags;
		point_write->extended_classification = point_read->extended_classification;
		point_write->extended_return_number = point_read->extended_return_number;
		point_write->extended_number_of_returns = point_read->extended_number_of_returns;
		point_write->extended_scan_angle = point_read->extended_scan_angle;

		if (point_read->num_extra_bytes)
		{
			memcpy(point_write->extra_bytes, point_read->extra_bytes, point_read->num_extra_bytes);
		}

		laszip_write_point(writer);
	}
	std::cout << count << endl;
	laszip_close_writer(writer);
	laszip_destroy(writer);

	laszip_close_reader(lasreader);


	laszip_destroy(lasreader);




}


