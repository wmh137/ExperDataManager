#include<io.h>
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<algorithm>

#include<FL/Fl.H>
#include<FL/Enumerations.H>
#include<FL/Fl_Window.H>
#include<FL/Fl_Menu_Item.H>
#include<FL/Fl_Menu_Bar.H>
#include<FL/Fl_Menu_Button.H>
#include<FL/Fl_Button.H>
#include<FL/Fl_Output.H>
#include<FL/Fl_Box.H>
#include<FL/Fl_File_Chooser.H>
#include<FL/Fl_Table_Row.H>
#include<FL/fl_draw.H>

//Variable

class DataClass
{
public:
	std::string DataPath;
	std::vector<std::string> DataType;
	size_t pathInd;
	void assign(DataClass data)
	{
		DataPath = data.DataPath;
		DataType = data.DataType;
	}
	void clear()
	{
		DataPath = "";
		std::vector<std::string>().swap(DataType);
	}
};

class DataBaseClass
{
public:
	std::string DSNAME;
	std::string DSPATH;
	std::vector<std::string> DataPathList;
	std::vector<std::string> TypeName;
	std::vector<DataClass> Data;

	std::string gettypename(size_t i)
	{
		return TypeName[i];
	}
	std::string getdatapath(size_t i)
	{
		return Data[i].DataPath;
	}
	std::string gettype(size_t i, size_t j)
	{
		return Data[i].DataType[j];
	}
	//0 for normal, 1 for added, 2 for missing
	char getstatus(size_t i)
	{
		std::string path = DSPATH + Data[i].DataPath;
		if (!_access(path.c_str(), 0))
		{
			for (size_t j = 0; j < Data[i].DataType.size(); j++)
			{
				if (Data[i].DataType[j] != "")
				{
					return 0;
				}
			}
			return 1;
		}
		else
		{
			return 2;
		}
	}

	void addpath(std::string pname)
	{
		auto iter = std::find(DataPathList.begin(), DataPathList.end(), pname);
		if (iter == DataPathList.end())
		{
			DataPathList.push_back(pname);
		}
	}
	void addtype(std::string tname)
	{
		auto iter = std::find(TypeName.begin(), TypeName.end(), tname);
		if (iter == TypeName.end())
		{
			TypeName.push_back(tname);
			for (size_t i = 0; i < Data.size(); i++)
			{
				Data[i].DataType.push_back("");
			}
		}
	}
	void adddata(DataClass data)
	{
		Data.push_back(data);
	}

	void reducepath(std::string pname)
	{
		auto iter = std::find(DataPathList.begin(), DataPathList.end(), pname);
		if (iter != DataPathList.end())
		{
			DataPathList.erase(iter);
		}
	}
	void reducetype(std::string tname)
	{
		auto iter = std::find(TypeName.begin(), TypeName.end(), tname);
		size_t ind = std::distance(TypeName.begin(), iter);
		if (iter != TypeName.end())
		{
			TypeName.erase(iter);
			for (auto iterd = Data.begin(); iterd != Data.end(); iterd++)
			{
				iterd->DataType.erase(iterd->DataType.begin() + ind);
			}
		}
	}

	size_t findpath(std::string path)
	{
		auto iter = std::find(DataPathList.begin(), DataPathList.end(), path);
		return std::distance(DataPathList.begin(), iter);
	}
	size_t finddatapath(std::string path)
	{

		for (size_t i = 0; i < Data.size(); i++)
		{
			if (Data[i].DataPath == path)
			{
				return i;
			}
		}
		return UINT32_MAX;
	}

	size_t size()
	{
		return Data.size();
	}
	size_t pathnum()
	{
		return DataPathList.size();
	}
	size_t typenum() {
		return TypeName.size();
	}
	void clear()
	{
		DSNAME = "";
		DSPATH = "";
		std::vector<std::string>().swap(DataPathList);
		std::vector<std::string>().swap(TypeName);
		std::vector<DataClass>().swap(Data);
	}
};
DataBaseClass DataBase;

class RowTable :public Fl_Table_Row
{
public:
	RowTable(int x, int y, int w, int h, const char* l = 0) :Fl_Table_Row(x, y, w, h, l)
	{
	}
	void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
	{
		switch (context)
		{
		case CONTEXT_STARTPAGE:
			return;

		case CONTEXT_COL_HEADER:
			fl_push_clip(X, Y, W, H);
			{
				fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
				fl_color(FL_BLACK);
				fl_font(FL_COURIER, 16);
				if (!C)
				{
					fl_draw("Path", X, Y, W, H, FL_ALIGN_CENTER);
				}
				else
				{
					fl_draw(DataBase.gettypename(C - 1).c_str(), X, Y, W, H, FL_ALIGN_CENTER);
				}
			}
			fl_pop_clip();
			return;

		case CONTEXT_ROW_HEADER:
			fl_push_clip(X, Y, W, H);
			{
				fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
				fl_color(FL_BLACK);
				fl_font(FL_COURIER, 15);
				fl_draw(std::to_string(R + 1).c_str(), X, Y, W, H, FL_ALIGN_CENTER);
			}
			fl_pop_clip();
			return;

		case CONTEXT_CELL:
		{
			fl_push_clip(X, Y, W, H);
			{
				fl_color(row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);
				fl_color(ColorStatus[DataBase.getstatus(R)]);
				fl_font(FL_HELVETICA, 18);
				std::string showstr;
				if (!C)
				{
					showstr = DataBase.getdatapath(R);

				}
				else
				{
					showstr = DataBase.gettype(R, C - 1);
				}
				fl_draw(showstr.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
				fl_color(color());
				fl_rect(X, Y, W, H);
			}
			fl_pop_clip();
			return;
		}
		case CONTEXT_TABLE:
		case CONTEXT_ENDPAGE:
		case CONTEXT_RC_RESIZE:
		case CONTEXT_NONE:
			return;
		}
	}
	Fl_Color ColorStatus[3] = { FL_BLACK,FL_BLUE,FL_RED };// black, blue, red
	void settable()
	{
		clear();
		rows(DataBase.size());
		if (DataBase.size())
		{
			cols(DataBase.typenum() + 1);
			col_header_height(25);
			row_header_width(40);
			col_width_all(90);
			row_height_all(25);
			col_width(0, 280);
		}
		else
		{
			cols(0);
		}
	}
};

//Widgwts

Fl_Window* window = new Fl_Window(1000, 750);
Fl_Menu_Bar* MB_top = new Fl_Menu_Bar(0, 0, 1200, 30);
Fl_Menu_Button* MBtn_Sort = new Fl_Menu_Button(0, 30, 120, 30, "Sort");
Fl_Menu_Button* MBtn_Find = new Fl_Menu_Button(200, 30, 120, 30, "Find");
Fl_Input* Input_Find = new Fl_Input(360, 30, 180, 30);
Fl_Input* Input_Type = new Fl_Input(700, 30, 200, 30);
Fl_Button* Btn_Delete = new Fl_Button(900, 30, 100, 30, "Delete");
Fl_File_Chooser* Fch = new Fl_File_Chooser("", "*.edm", Fl_File_Chooser::SINGLE, "");
Fl_File_Chooser* Fch_new = new Fl_File_Chooser("", "*.edm", Fl_File_Chooser::DIRECTORY, "");
RowTable* Table = new RowTable(0, 60, 1000, 400);



//Backend

void DataBaseImport(std::string path)
{
	DataBase.clear();
	DataClass newData;
	std::string instr;
	std::string typestr, valuestr;
	int ind;
	std::ifstream in(path);

	char Status;// 0 for unknown, 1 for pathlist, 2 for typename, 3 for import, 4 for save
	in >> instr;
	Status = 0;
	while (!in.eof())
	{
		switch (Status)
		{
		case 1:
			DataBase.addpath(instr);
			break;
		case 2:
			DataBase.addtype(instr);
			break;
		case 3:
			if (newData.DataType.size() < DataBase.typenum())
			{
				newData.DataType.resize(DataBase.typenum());
			}
			ind = instr.find("=");
			typestr = instr.substr(0, ind);
			valuestr = instr.substr(ind + 1);
			if (typestr == "PATH")
			{
				newData.DataPath = valuestr;
			}
			else
			{
				for (size_t i = 0; i < DataBase.typenum(); i++)
				{
					if (DataBase.TypeName[i] == typestr)
					{
						newData.DataType[i] = valuestr;
						break;
					}
				}
			}
			break;
		case 4:
			if (newData.DataPath.size())
			{
				DataBase.adddata(newData);
			}
			newData.clear();
			Status = 3;
			break;
		case 0:
			if (instr == "DATAPATH")
			{
				Status = 1;
			}
			else if (instr == "TYPENAME")
			{
				Status = 2;
			}
			else if (instr == "DATABASE")
			{
				Status = 3;
			}
		}
		in >> instr;
		if (instr == "END")
		{
			Status = 0;
		}
		else if (instr == "\\")
		{
			Status = 4;
		}
	}
	in.close();
}
void DataBaseExport(std::string path)
{
	std::string strtemp;
	std::ofstream out(path);
	out << "DATAPATH\n";
	for (size_t i = 0; i < DataBase.DataPathList.size(); i++)
	{
		out << DataBase.DataPathList[i] << " ";
	}
	out << std::endl << "END\nTYPENAME\n";
	for (size_t i = 0; i < DataBase.typenum(); i++)
	{
		out << DataBase.TypeName[i] << " ";
	}
	out << "\nEND\nDATABASE\n";
	for (size_t i = 0; i < DataBase.size(); i++)
	{
		out << "PATH=" << DataBase.Data[i].DataPath << " ";
		for (size_t j = 0; j < DataBase.typenum(); j++)
		{
			strtemp = DataBase.Data[i].DataType[j];
			if (strtemp.size())
			{
				out << DataBase.TypeName[j] << "=" << strtemp << " ";
			}
		}
		out << "\\\n";
	}
	out << "END";
	out.close();
}
void DataBaseCheck(std::string path)
{
	std::string datapath;
	intptr_t Handle;
	struct _finddata_t FileInfo;
	size_t ind;
	for (size_t i = 0; i < DataBase.pathnum(); i++)
	{
		datapath = path + DataBase.DataPathList[i];
		if ((Handle = _findfirst((datapath + "/*").c_str(), &FileInfo)) != -1)
		{
			do
			{
				if (FileInfo.attrib == _A_ARCH)
				{
					ind = DataBase.finddatapath(DataBase.DataPathList[i] + "/" + FileInfo.name);
					if (ind == UINT32_MAX)
					{
						DataClass newData;
						newData.DataPath = DataBase.DataPathList[i] + "/" + FileInfo.name;
						newData.DataType.resize(DataBase.typenum());
						DataBase.adddata(newData);
					}
				}
			} while (_findnext(Handle, &FileInfo) == 0);
			_findclose(Handle);
		}
	}
	auto iter = DataBase.Data.begin();
	while (iter != DataBase.Data.end())
	{
		bool fit = FALSE;
		for (size_t j = 0; j < DataBase.pathnum(); j++)
		{
			if (iter->DataPath.substr(0, DataBase.DataPathList[j].size() + 1) == (DataBase.DataPathList[j] + "/"))
			{
				if (iter->DataPath.substr(DataBase.DataPathList[j].size() + 1).find("/") == std::string::npos)
				{
					iter->pathInd = j;
					fit = TRUE;
					break;
				}
			}
		}
		if (fit)
		{
			iter++;
		}
		else
		{
			iter = DataBase.Data.erase(iter);
		}
	}
}

bool PathListSort(DataClass data1, DataClass data2)
{
	return data1.pathInd < data2.pathInd;
}
bool PathSort(DataClass data1, DataClass data2)
{
	return data1.DataPath < data2.DataPath;
}
bool TypeSort(DataClass data1, DataClass data2)
{
	return data1.DataType[MBtn_Sort->value() - 2] < data2.DataType[MBtn_Sort->value() - 2];
}

bool PathListFind(DataClass data1, DataClass data2)
{
	std::string findstr = Input_Find->value();
	return (DataBase.DataPathList[data1.pathInd] == findstr) > (DataBase.DataPathList[data2.pathInd] == findstr);
}
bool PathFind(DataClass data1, DataClass data2)
{
	std::string findstr = Input_Find->value();
	return (data1.DataPath == findstr) > (data2.DataPath == findstr);
}
bool TypeFind(DataClass data1, DataClass data2)
{
	std::string findstr = Input_Find->value();
	size_t ind = MBtn_Find->value() - 2;
	return (data1.DataType[ind] == findstr) > (data2.DataType[ind] == findstr);
}

//Callback

void New_cb() {
	std::string path, strtemp;
	Fch_new->show();
	while (Fch_new->visible())
	{
		Fl::wait();
	}
	if (Fch_new->count())
	{
		const char* name = fl_input("DataBase Name", "DS");
		if (name)
		{
			path = Fch_new->value();
			if (path.back() != '/')
			{
				path.append("/");
			}
			strtemp = path + name + ".edm";
			if (_access(strtemp.c_str(), 0))
			{
				DataBase.clear();
				DataBase.DSPATH = path;
				DataBase.DSNAME = name;
				std::ofstream newDS(strtemp);
				newDS.close();
				Table->settable();
				std::string winlabel = "ExperDataManager ";
				winlabel.append(DataBase.DSPATH + DataBase.DSNAME + ".edm");
				window->label(winlabel.c_str());
				MBtn_Sort->clear();
				MBtn_Find->clear();
				Input_Type->value("");
				Input_Type->deactivate();
			}
			else
			{
				fl_message_font(FL_COURIER, 16);
				fl_message_title("Warning");
				fl_message("The DataBase exists!");
			}
		}
	}

}
void Open_Fch_cb()
{
	std::string path;
	Fch->show();
	while (Fch->visible())
	{
		Fl::wait();
	}
	if (Fch->count())
	{
		path.assign(Fch->value());
		std::ifstream in(Fch->value());
		if (in.is_open())
		{
			in.close();
			DataBaseImport(path);
			size_t ind = path.find_last_of('/') + 1;
			DataBase.DSPATH = path.substr(0, ind);
			DataBase.DSNAME = path.substr(ind);
			DataBase.DSNAME = DataBase.DSNAME.substr(0, DataBase.DSNAME.length() - 4);
			DataBaseCheck(DataBase.DSPATH);
			Table->settable();
			MBtn_Sort->clear(); MBtn_Find->clear();
			MBtn_Sort->add("PathList"); MBtn_Find->add("PathList");
			MBtn_Sort->add("Path"); MBtn_Find->add("Path");
			for (auto iter = DataBase.TypeName.begin(); iter != DataBase.TypeName.end(); iter++)
			{
				MBtn_Sort->add((*iter).c_str());
				MBtn_Find->add((*iter).c_str());
			}
			std::string winlabel = "ExperDataManager ";
			winlabel.append(DataBase.DSPATH + DataBase.DSNAME + ".edm");
			window->label(winlabel.c_str());
			Input_Type->value("");
		}
	}
}
void Save_cb()
{
	if (DataBase.DSPATH.size())
	{
		DataBaseExport(DataBase.DSPATH + DataBase.DSNAME + ".edm");
	}
}
void Rfs_cb()
{
	if (DataBase.DSPATH.size())
	{
		DataBaseCheck(DataBase.DSPATH);
		Table->rows(DataBase.size());
		Table->redraw();
	}
}
void Exit_cb()
{
	exit(0);
}

void Show_Path_cb()
{
	std::string showstr;
	for (auto iter = DataBase.DataPathList.begin(); iter != DataBase.DataPathList.end(); iter++)
	{
		showstr.append(*iter + "\n");
	}
	fl_message_font(FL_COURIER, 16);
	fl_message_title("Path");
	fl_message(showstr.c_str());
}
void Add_Path_cb()
{
	if (DataBase.DSPATH.size())
	{
		const char* input = fl_input("Add Path", "");
		if (input) {
			DataBase.addpath(input);
			DataBaseCheck(DataBase.DSPATH);
			Table->settable();
		}
	}
}
void Reduce_Path_cb()
{
	if (DataBase.DSPATH.size())
	{
		const char* input = fl_input("Reduce Path", "");
		if (input) {
			DataBase.reducepath(input);
			DataBaseCheck(DataBase.DSPATH);
			Table->settable();
		}
	}
}
void Add_Type_cb()
{
	if (DataBase.DSPATH.size())
	{
		const char* input = fl_input("Add Type", "");
		if (input) {
			DataBase.addtype(input);
			DataBaseCheck(DataBase.DSPATH);
			MBtn_Sort->clear(); MBtn_Find->clear();
			MBtn_Sort->add("PathList"); MBtn_Find->add("PathList");
			MBtn_Sort->add("Path"); MBtn_Find->add("Path");
			for (auto iter = DataBase.TypeName.begin(); iter != DataBase.TypeName.end(); iter++)
			{
				MBtn_Sort->add((*iter).c_str());
				MBtn_Find->add((*iter).c_str());
			}
			Table->settable();
		}
	}
}
void Reduce_Type_cb()
{
	if (DataBase.DSPATH.size())
	{
		const char* input = fl_input("Reduce Type", "");
		if (input) {
			DataBase.reducetype(input);
			DataBaseCheck(DataBase.DSPATH);
			MBtn_Sort->clear(); MBtn_Find->clear();
			MBtn_Sort->add("PathList"); MBtn_Find->add("PathList");
			MBtn_Sort->add("Path"); MBtn_Find->add("Path");
			for (auto iter = DataBase.TypeName.begin(); iter != DataBase.TypeName.end(); iter++)
			{
				MBtn_Sort->add((*iter).c_str());
				MBtn_Find->add((*iter).c_str());
			}
			Table->settable();
		}
	}
}

void Table_cb(Fl_Widget*, void* rcind)
{
	size_t rowInd, colInd = 0;
	for (rowInd = 0; rowInd < DataBase.size(); rowInd++)
	{
		if (Table->row_selected(rowInd))
		{
			break;
		}
	}
	((size_t*)rcind)[0] = rowInd;
	if (rowInd != DataBase.size())
	{
		for (colInd = 0; colInd < DataBase.typenum() + 1; colInd++)
		{
			if (Table->is_selected(rowInd, colInd))
			{
				break;
			}
		}
		((size_t*)rcind)[1] = colInd;
		if (colInd != DataBase.typenum() + 1)
		{
			if (colInd == 0)
			{
				Input_Type->deactivate();
				Input_Type->value(DataBase.Data[rowInd].DataPath.c_str());
			}
			else
			{
				Input_Type->activate();
				Input_Type->value(DataBase.gettype(rowInd, colInd - 1).c_str());
			}
			if (DataBase.getstatus((rowInd)) == 2)
			{
				Btn_Delete->activate();
			}
		}
		else
		{
			Input_Type->deactivate();
			Input_Type->value("");
			Btn_Delete->deactivate();
		}
	}
	else
	{
		Input_Type->deactivate();
		Input_Type->value("");
		Btn_Delete->deactivate();
	}
	Table->redraw();
}
void MBtn_Sort_cb()
{
	switch (MBtn_Sort->value())
	{
	case 0:
		std::sort(DataBase.Data.begin(), DataBase.Data.end(), PathListSort);
		Table->redraw();
		break;
	case 1:
		std::sort(DataBase.Data.begin(), DataBase.Data.end(), PathSort);
		Table->redraw();
		break;
	default:
		std::sort(DataBase.Data.begin(), DataBase.Data.end(), TypeSort);
		Table->redraw();
		break;
	}
}
void MBtn_Find_cb()
{
	switch (MBtn_Find->value())
	{
	case 0:
		sort(DataBase.Data.begin(), DataBase.Data.end(), PathListFind);
		Table->redraw();
		break;
	case 1:
		sort(DataBase.Data.begin(), DataBase.Data.end(), PathFind);
		Table->redraw();
		break;
	default:
		sort(DataBase.Data.begin(), DataBase.Data.end(), TypeFind);
		Table->redraw();
		break;
	}
}
void Input_Type_cb(Fl_Widget*, void* rcind)
{
	size_t rowInd, colInd;
	rowInd = ((size_t*)rcind)[0];
	colInd = ((size_t*)rcind)[1];
	DataBase.Data[rowInd].DataType[colInd - 1] = Input_Type->value();
	Table->redraw();
}
void Btn_Delete_cb(Fl_Widget*, void* rcind)
{
	DataBase.Data.erase(DataBase.Data.begin() + ((size_t*)rcind)[0]);
	Table->rows(DataBase.size());
	Table->redraw();
}


int main(int argc, char** argv)
{

	//Variable

	size_t rcind[2];

	window->begin();
	//Setting

	window->box(FL_FLAT_BOX);
	window->label("ExperDataManager");


	Fl_Menu_Item MI_top[] = {
		{"File", 0, 0, 0, FL_SUBMENU},
		{"New",FL_COMMAND + 'n', (Fl_Callback*)New_cb},
		{"Open", FL_COMMAND + 'o', (Fl_Callback*)Open_Fch_cb},
		{"Save", FL_COMMAND + 's', (Fl_Callback*)Save_cb},
		{"Refrash", FL_F + 5, (Fl_Callback*)Rfs_cb},
		{"Exit", FL_Escape, (Fl_Callback*)Exit_cb},
		{0},
		{"DataBase", 0, 0, 0, FL_SUBMENU},
		{"PathList", 0, 0, 0,FL_SUBMENU},
		{"Show", 0, (Fl_Callback*)Show_Path_cb},
		{"Add", 0, (Fl_Callback*)Add_Path_cb},
		{"Reduce", 0, (Fl_Callback*)Reduce_Path_cb},
		{0},
		{"Type", 0, 0, 0, FL_SUBMENU},
		{"Add", 0, (Fl_Callback*)Add_Type_cb},
		{"Reduce", 0, (Fl_Callback*)Reduce_Type_cb},
		{0},
		{0},
		{"Option", 0, 0, 0, FL_SUBMENU},
		{0},
		{0}
	};
	MB_top->copy(MI_top);
	MB_top->box(FL_FLAT_BOX);
	MB_top->color(FL_WHITE);
	MB_top->textsize(16);
	MB_top->textfont(FL_COURIER);

	Fch->textsize(16);
	Fch_new->textsize(16);

	Table->box(FL_BORDER_FRAME);
	Table->col_resize(1);
	Table->row_resize(1);
	Table->col_header(1);
	Table->row_header(1);
	Table->type(Table->SELECT_SINGLE);
	Table->selection_color(43);
	Table->when(FL_WHEN_RELEASE | FL_WHEN_CHANGED);

	//MBtn_Sort->box(FL_BORDER_BOX);
	MBtn_Sort->labelfont(FL_COURIER);
	MBtn_Sort->labelsize(16);
	MBtn_Sort->textfont(FL_COURIER);
	MBtn_Sort->textsize(16);

	MBtn_Find->labelfont(FL_COURIER);
	MBtn_Find->labelsize(16);
	MBtn_Find->textfont(FL_COURIER);
	MBtn_Find->textsize(16);

	Input_Find->textsize(16);
	Input_Find->textfont(FL_COURIER);

	Input_Type->textsize(16);
	Input_Type->textfont(FL_COURIER);
	Input_Type->deactivate();
	Input_Type->when(FL_WHEN_ENTER_KEY);

	Btn_Delete->labelsize(16);
	Btn_Delete->labelfont(FL_COURIER);
	Btn_Delete->deactivate();

	//Callback

	Table->callback((Fl_Callback*)(Table_cb), rcind);
	MBtn_Sort->callback((Fl_Callback*)(MBtn_Sort_cb));
	MBtn_Find->callback((Fl_Callback*)MBtn_Find_cb);
	Input_Type->callback((Fl_Callback*)Input_Type_cb, rcind);
	Btn_Delete->callback((Fl_Callback*)Btn_Delete_cb, rcind);

	//Backend


	window->end();
	window->show(argc, argv);
	return Fl::run();
}