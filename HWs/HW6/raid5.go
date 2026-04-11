package main

// RAID 5 = parity rotates across disks
type RAID5 struct {
	disks []*Disk
}

// Decide which disk stores parity for a stripe
func getParityDisk(stripe int) int {
	return stripe % NumDisks
}

func (r *RAID5) Write(blockNum int, data []byte) {

	stripe := blockNum / (NumDisks - 1)
	parityDisk := getParityDisk(stripe)

	dataIndex := blockNum % (NumDisks - 1)

	// Skip parity disk
	var disk int
	if dataIndex >= parityDisk {
		disk = dataIndex + 1
	} else {
		disk = dataIndex
	}

	oldData := r.disks[disk].Read(stripe)
	oldParity := r.disks[parityDisk].Read(stripe)

	newParity := xor(xor(oldParity, oldData), data)

	r.disks[disk].Write(stripe, data)
	r.disks[parityDisk].Write(stripe, newParity)
}

func (r *RAID5) Read(blockNum int) []byte {

	stripe := blockNum / (NumDisks - 1)
	parityDisk := getParityDisk(stripe)

	dataIndex := blockNum % (NumDisks - 1)

	var disk int
	if dataIndex >= parityDisk {
		disk = dataIndex + 1
	} else {
		disk = dataIndex
	}

	return r.disks[disk].Read(stripe)
}
