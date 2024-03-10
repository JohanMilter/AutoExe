#include <iostream>
#include <algorithm>

//File creation
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <numeric>
#include <filesystem> 
static size_t Count(const std::string& input, const std::string& instance)
{
	size_t delimeterLength = instance.length();
	size_t count = 0;
	size_t pos = 0;
	while ((pos = input.find(instance, pos)) != std::string::npos)
	{
		++count;
		pos += delimeterLength;
	}
	return count;
}
static std::vector<std::string> Split(const std::string& input, const std::string& delimiter)
{
	size_t count = Count(input, delimiter);
	std::vector<std::string> parts(count + 1);
	size_t delimeterLength = delimiter.length();
	size_t start = 0;
	size_t end = 0;
	for (size_t i = 0; i < count; i++)
	{
		end = input.find(delimiter, start);
		parts[i] = input.substr(start, end - start);
		start = end + delimeterLength;
	}
	parts[count] = input.substr(start);
	return parts;
}
static std::string Join(const std::vector<std::string> vec, const std::string& delimiter)
{
	return vec.empty() ? ""
		: std::accumulate(std::next(vec.begin()), vec.end(), vec[0], [&delimiter](const std::string& a, const std::string& b)
	{
		return a + delimiter + b;
	});
}

#pragma region Filepath
class IFilepath
{
private:
protected:
	std::string m_Path;
	std::vector<std::string> m_Folders;
public:
	//Create
	virtual bool Create() const = 0;
	//Path
	virtual void Path(const std::string& path) = 0;
	virtual std::string Path() const = 0;
	//Folders
	virtual void Folders(const std::vector<std::string>& path) = 0;
	virtual std::vector<std::string> Folders() const = 0;
};
class Filepath : public IFilepath
{
private:
protected:
public:
	//Create
	bool Create() const override
	{
		return false;
	}
	//Path
	void Path(const std::string& path) override
	{
		size_t lastOf = path.find_last_of('\\');
		m_Path = (path.substr(lastOf + 1).find_first_of('.') != std::string::npos ? path.substr(0, lastOf) : path);
		m_Folders = Split(m_Path, "\\");
	}
	std::string Path() const override
	{
		return m_Path;
	}
	//Folders
	void Folders(const std::vector<std::string>& folders) override
	{
		m_Folders = folders;
		if (m_Folders[m_Folders.size() - 1].find('.') != std::string::npos)
			m_Folders.pop_back();
		m_Path = Join(m_Folders, "\\");
	}
	std::vector<std::string> Folders() const override
	{
		return m_Folders;
	}
};
#pragma endregion
#pragma region Folder
class IFolder : public Filepath
{
private:
protected:
public:
};
class Folder : public IFolder
{
private:
protected:
public:
};
#pragma endregion
#pragma region File
class IFile : public Filepath
{
private:
protected:
	std::string m_Content;
	std::string m_Filepath;
	std::string m_Filename;
	std::string m_FileExtension;
public:
	bool Create() const override
	{
		std::ofstream file{ m_Filepath };
		if (file.is_open())
		{
			file << m_Content;
			file.close();
			return true;
		}
		return false;
	}
	//Compile
	virtual bool Compile() const = 0;
	//Content
	virtual void Content(const std::string& content) = 0;
	virtual std::string Content() const = 0;
	//Filepath
	virtual void Filepath(const std::string& filepath) = 0;
	virtual std::string Filepath() const = 0;
	//Filename
	virtual void Filename(const std::string& filename) = 0;
	virtual std::string Filename() const = 0;
	//Extension
	virtual void FileExtension(const std::string& fileExtension) = 0;
	virtual std::string FileExtension() const = 0;
};
class File : public IFile
{
private:
protected:
public:
	//Compile
	bool Compile() const override
	{
		std::string extension = m_FileExtension;
		for (char& c : extension)
			c = std::tolower(static_cast<unsigned char>(c));
		std::string command;
		if (!std::filesystem::exists(m_Filepath))
			return false;

		if (extension == ".cpp")
			command = "g++ " + m_Filepath + " -o " + m_Path + '\\' + m_Filename;
		system(command.c_str());
		return true;
	}
	//Content
	void Content(const std::string& content) override
	{
		m_Content = content;
	}
	std::string Content() const override
	{
		return m_Content;
	}
	//Filepath
	void Filepath(const std::string& filepath) override
	{
		size_t lastSlashPos = filepath.find_last_of("\\");
		if (lastSlashPos != std::string::npos)
		{
			size_t firstDotPos = filepath.find_first_of(".");
			m_Filename = filepath.substr(lastSlashPos + 1, firstDotPos - lastSlashPos - 1);
			m_FileExtension = filepath.substr(firstDotPos);
			m_Path = filepath.substr(0, lastSlashPos);
		}
		m_Filepath = filepath;
	}
	std::string Filepath() const override
	{
		return m_Filepath;
	}
	//Path
	void Path(const std::string& path) override
	{
		m_Path = path;
		m_Filepath = m_Path + '\\' + m_Filename;
	}
	std::string Path() const override
	{
		return m_Path;
	}
	//Filename
	void Filename(const std::string& filename) override
	{
		m_Filename = filename;
		m_Filepath = m_Path + '\\' + m_Filename + m_FileExtension;
	}
	std::string Filename() const override
	{
		return m_Filename;
	}
	//Extension
	void FileExtension(const std::string& fileExtension) override
	{
		m_FileExtension = fileExtension;
		m_Filepath = m_Path + '\\' + m_Filename + m_FileExtension;
	}
	std::string FileExtension() const override
	{
		return m_FileExtension;
	}
};
#pragma endregion

#pragma region Module
class IBuild
{
private:
protected:
public:
	virtual std::string Build() const = 0;
};

class IInclude : public IBuild
{
private:
protected:
	std::string m_Include;
public:
	std::string Build() const override
	{
		return "#include <" + m_Include + ">\n";
	}
};
class Include : public IInclude
{
private:
protected:
public:
	Include(const std::string& include)
	{
		m_Include = include;
	}
};
class IIncludes : public IBuild
{
private:
protected:
	std::vector<Include> m_Includes;
public:
	virtual void Add(const Include& module) = 0;
	std::string Build() const override
	{
		std::string includes;
		for (const auto& include : m_Includes)
			includes += include.Build();
		return includes;
	}
};
class Includes : public IIncludes
{
private:
protected:
public:
	void Add(const Include& include) override
	{
		m_Includes.push_back(include);
	}
};
class IModule : public IBuild
{
private:
protected:
	std::string m_Module;
public:
	std::string Build() const override
	{
		return m_Module;
	}
};
class Module : public IModule
{
private:
protected:
public:
	Module(const std::string& module)
	{
		m_Module = module;
	}
};
class IModules : public IBuild
{
private:
protected:
	std::vector<Module> m_Modules;
public:
	virtual void Add(const Module& module) = 0;
	std::string Build() const override
	{
		std::string modules;
		for (const auto& module : m_Modules)
			modules += module.Build();
		return modules;
	}
};
class Modules : public IModules
{
private:
protected:
public:
	void Add(const Module& module) override
	{
		m_Modules.push_back(module);
	}
};
class IRefference : public IBuild
{
private:
protected:
	std::string m_Refference;
public:
	std::string Build() const override
	{
		return '\t' + m_Refference;
	}
};
class Refference : public IRefference
{
private:
protected:
public:
	Refference(const std::string& refference)
	{
		m_Refference = refference;
	}
};
class IRefferences : public IBuild
{
private:
protected:
	std::vector<Refference> m_Refferences;
public:
	virtual void Add(const Refference& refference) = 0;
	std::string Build() const override
	{
		std::string refferences;
		for (const auto& refference : m_Refferences)
			refferences += refference.Build();
		return refferences;
	}
};
class Refferences : public IRefferences
{
private:
protected:
public:
	void Add(const Refference& refference) override
	{
		m_Refferences.push_back(refference);
	}
};

class AutoExeContent : public IBuild
{
private:
protected:
	Includes m_Includes;
	Modules m_Modules;
	Refferences m_Refferences;
public:
	std::string Build() const override
	{
		return m_Includes.Build() + m_Modules.Build() + "int main()\n{\n" + m_Refferences.Build() + "}\n";
	}
	void Add(const Include& include)
	{
		m_Includes.Add(include);
	}
	void Add(const Module& module)
	{
		m_Modules.Add(module);
	}
	void Add(const Refference& refference)
	{
		m_Refferences.Add(refference);
	}
};
#pragma endregion


int main()
{
	auto start = std::chrono::steady_clock::now();
	AutoExeContent autoExeContent;
	autoExeContent.Add(
		Include{ "iostream" }
	);
	autoExeContent.Add(
		Module
		{
		"void Start(const std::string& text)\n"
		"{\n"
		"\tstd::cout << text << std::endl;\n"
		"}\n"
		}
	);
	autoExeContent.Add(
		Module
		{
		"void Start(const int number)\n"
		"{\n"
		"\tstd::cout << number << std::endl;\n"
		"}\n"
		}
	);
	autoExeContent.Add(
		Refference{ "Start(\"Hello World!\");\n" }
	);
	autoExeContent.Add(
		Refference{ "Start(5);\n" }
	);
	autoExeContent.Add(
		Refference{ "system(\"pause\");\n" }
	);
	File file;
	file.Filepath("C:\\Users\\Ejer\\Desktop\\Coding\\C++\\AutoExe\\AutoExe\\Output\\Program.cpp");
	file.Content(autoExeContent.Build());
	file.Create();
	std::cout << file.Compile() << std::endl;






	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << "It took " << duration << "us (" << duration / 1000 << "ms)\n";
}

