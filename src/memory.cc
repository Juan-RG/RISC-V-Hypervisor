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

	std::cout << _binary[0] << "\n";
	
	
	// read elf header
	_ehdr = *reinterpret_cast<Elf32_Ehdr*>(_binary.data());
		
		// ensure riscv32
	if(_ehdr.e_machine != EM_ARM){
		exit(0);
	}
    // ensure the binary has a correct program table
	//check if al data is correc
	if(_ehdr.e_phoff == 0){ // ask dario if program header table can be 0. in my opinion no
		exit(0);
	}
	if(_ehdr.e_phnum == 0){
		exit(0);
	}

	//check size because register depends of it
	int numEntries = 0;
	Elf32_Shdr _shdr;
	if(_ehdr.e_phnum >= PN_XNUM){
		//Check error
		if(_ehdr.e_shoff == 0){
			exit(0);
		}
		//cast and get data
		Elf32_Shdr _shdr = *reinterpret_cast<Elf32_Shdr*>(_binary.data() + _ehdr.e_shoff);
		numEntries = _shdr.sh_info;
	}else{
		numEntries = _ehdr.e_phnum;
	}
	// load sections in memory																									/**Es esto ?¿?¿?¿ I dont sure
	for(int i = 0; i < numEntries; i++){
		//binario +offset + offsetelement
		Elf32_Phdr phdr = *reinterpret_cast<Elf32_Phdr*>(_binary.data() + _ehdr.e_phoff + (_ehdr.e_phentsize * i));
		_phdr.push_back(phdr);
	}

    // entry point
	if(_ehdr.e_entry == 0){
		exit(0);
	}
		// load sections in memory
  	
	  // read ELF program header table,
  	// ... to be completed

}



bool elf_check_supported(Elf32_Ehdr *hdr) {
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		exit(2);
		//ERROR("Unsupported ELF File Class.\n");
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		exit(2);
		//ERROR("Unsupported ELF File byte order.\n");
		return false;
	}
	if(hdr->e_machine != EM_386) {
		exit(2);
		//ERROR("Unsupported ELF File target.\n");
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		exit(2);
		//ERROR("Unsupported ELF File version.\n");
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		exit(2);
		//ERROR("Unsupported ELF File type.\n");
		return false;
	}
	return true;
}
