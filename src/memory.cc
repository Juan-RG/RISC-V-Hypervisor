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
			segment firstSegment = segment(phdr.p_offset, phdr.p_filesz);
			std::cout << "segmento  " << firstSegment._initial_address << std::endl;
			_segments.push_back(firstSegment);
		}
	}



		  // read ELF program header table,
	if(_ehdr.e_shoff == 0){
			std::cerr << "Invalid e_shoff == 0 \n";
			exit(0);
	}	  
	
	_shdr = *reinterpret_cast<Elf32_Shdr*>(_binary.data() + _ehdr.e_shoff);

	if(_shdr.sh_name != 1){
		std::cout << "Fin "<< std::endl;
	}
	  // ... to be completed



/* Torres version v1	
	auto offset = _ehdr.e_phoff;
	for(int i = 0; i < numEntries; i++){
		//binario +offset + offsetelement
		Elf32_Phdr phdr = *reinterpret_cast<Elf32_Phdr*>(_binary.data() + offset + (_ehdr.e_phentsize * i));
		_phdr.push_back(phdr);
		if (phdr.p_type == PT_PHDR){
			offset = phdr.p_offset - (_ehdr.e_phentsize * (i + 1));
		}
	}
*/

/* Juan segmentos v1
	for(size_t i = 0; i < _phdr.size(); i++){
		Elf32_Phdr phdr = _phdr[i];
		if(phdr.p_type == PT_LOAD){
			//std::vector<uint8_t> _raw_data(_binary.begin() + phdr.p_offset, _binary.begin() + phdr.p_offset + phdr.p_filesz);
			segment seg = segment(static_cast<address_t>(phdr.p_offset), phdr.p_filesz);
		    _segments.push_back(seg);

		}else{
			std::cout << "Segment != PT_LOAD" << std::endl;
			continue;
		}
	
		long sizeSegment = 0;
		if(phdr.p_memsz > phdr.p_filesz){
			std::cout << "Entro p_memsz > p_filesz" << std::endl;
			sizeSegment = phdr.p_filesz;
			sizeSegment = sizeSegment | ((long)phdr.p_memsz << 32);
		}else{
			sizeSegment = phdr.p_filesz;
		}

	}*/

		// load sections in memory
  	
	  // read ELF program header table,
  	// ... to be completed

}