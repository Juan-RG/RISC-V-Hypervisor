#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>

#include <memory.hh>

using namespace mem;

void
memory::load_binary(const std::string& binfile)
{
	using ifile_iter = std::istream_iterator<uint8_t>;

	std::ifstream ifile(binfile, std::ios::binary);

	if (ifile.is_open() == false) {
		std::cerr << "Unable to open "<< binfile << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// copy the binary into memory
	// Stop eating new lines in binary mode!!!
	ifile.unsetf(std::ios::skipws);

	ifile.seekg(0, std::ios::end); //Set position in input sequence
	const auto ifsize = ifile.tellg(); //Get position in input sequence 
	ifile.seekg(0, std::ios::beg);//Set position in input sequence

	_binary.reserve(ifsize);
	_binary.insert(_binary.begin(),
			ifile_iter(ifile),
			ifile_iter());
	ifile.close();

	//std::cout << _binary[0] << "\n";
	
	
	// read elf header
	_ehdr = *reinterpret_cast<Elf32_Ehdr*>(_binary.data());
		

 
    // ensure the binary has a correct program table
	if (
		_ehdr.e_ident[EI_MAG0] != ELFMAG0 || _ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
		_ehdr.e_ident[EI_MAG2] != ELFMAG2 || _ehdr.e_ident[EI_MAG3] != ELFMAG3
	   ) {
		std::cerr << "Unexpected magic number found.\n";
		//exit(0);
	}
	if (_ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
		std::cerr << "Unsupported ELF File Class.\n";
		//exit(0);
	}
	if (_ehdr.e_ident[EI_VERSION] != EV_CURRENT) {
		std::cerr << "Invalid version\n";
		//exit(0);
	}
	if (_ehdr.e_machine != EM_RISCV) {
		std::cerr << "Not targeting RISCV.\n";
		//exit(0);
	}
	//si es exec o shared good
	if (!(_ehdr.e_type == ET_EXEC && _ehdr.e_type == ET_DYN)) {
		std::cerr << "Non executable file.\n";
		//exit(0);
	} // else ELF Magic number is ok
	
	if (_ehdr.e_version != EV_CURRENT) {
		std::cerr << "Invalid version 2 \n";
		//exit(0);
	}
	

	//check if al data is correc
	if(_ehdr.e_phoff == 0){ // ask dario if program header table can be 0. in my opinion no
		std::cerr << "Invalid phoff == 0 \n";
		//exit(0);
	}
	if(_ehdr.e_phnum == 0){
		std::cerr << "Invalid e_phnum == 0 \n";
		//exit(0);
	}
   
		//check size because register depends of it
	int numEntries = 0;
	Elf32_Shdr _shdr;
	if(_ehdr.e_phnum >= PN_XNUM){
		//Check error
		if(_ehdr.e_shoff == 0){
			std::cerr << "Invalid phoff == 0 \n";
			exit(0);
		}
		//cast and get data
		_shdr = *reinterpret_cast<Elf32_Shdr*>(_binary.data() + _ehdr.e_shoff);
		numEntries = _shdr.sh_info;

	}else{
		numEntries = _ehdr.e_phnum;
	}
	// load sections headers in memory																									

	for(int i = 0; i < numEntries; i++){
		std::cout << "SEGMENTOS"<< std::endl;
		//binario +offset + offsetelement
		Elf32_Phdr phdr = *reinterpret_cast<Elf32_Phdr*>(_binary.data() + _ehdr.e_phoff + (_ehdr.e_phentsize * i));
		_phdr.push_back(phdr);
		if(phdr.p_type == PT_NULL){
			continue;
		}else if(phdr.p_type == PT_LOAD){
			if(phdr.p_filesz == 0){
				std::cerr << "Segment for program header p_filessz == 0 \n";
			}
			//Control de flags para saber si
			//PF_x - executable
			//PF_W - Writable
			//PF_R - Readable

			segment _segment = segment(phdr.p_vaddr, phdr.p_memsz);
			std::memcpy(reinterpret_cast<void *>( _segment._content.data() ),
						 _binary.data() + phdr.p_offset,
						 phdr.p_filesz );
			std::cout << "Saved segment, Initial vAddr: " << _segment._initial_address << ", vSize: " << phdr.p_memsz << std::endl;
			_segments.push_back(_segment);
		}
	}


}