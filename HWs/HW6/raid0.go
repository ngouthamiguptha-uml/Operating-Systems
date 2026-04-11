package main

// RAID 0 = striping across disks
type RAID0 struct {
	disks []*Disk
}

// Write: distribute blocks across disks
func (r *RAID0) Write(blockNum int, data []byte) {
	disk := blockNum % NumDisks
	block := blockNum / NumDisks

	r.disks[disk].Write(block, data)
}

// Read: same mapping
func (r *RAID0) Read(blockNum int) []byte {
	disk := blockNum % NumDisks
	block := blockNum / NumDisks

	return r.disks[disk].Read(block)
}
