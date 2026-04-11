package main

// RAID 1 = same data written to all disks
type RAID1 struct {
	disks []*Disk
}

// Write: write to ALL disks
func (r *RAID1) Write(blockNum int, data []byte) {
	for _, d := range r.disks {
		d.Write(blockNum, data)
	}
}

// Read: read from first disk (simple)
func (r *RAID1) Read(blockNum int) []byte {
	return r.disks[0].Read(blockNum)
}
