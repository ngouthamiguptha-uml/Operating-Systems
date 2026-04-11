package main

// XOR helper
func xor(a, b []byte) []byte {
	res := make([]byte, len(a))
	for i := range a {
		res[i] = a[i] ^ b[i]
	}
	return res
}

// RAID 4 = last disk stores parity
type RAID4 struct {
	disks []*Disk
}

// Write logic:
// parity = oldParity ^ oldData ^ newData
func (r *RAID4) Write(blockNum int, data []byte) {

	dataDisk := blockNum % (NumDisks - 1)
	block := blockNum / (NumDisks - 1)
	parityDisk := NumDisks - 1

	oldData := r.disks[dataDisk].Read(block)
	oldParity := r.disks[parityDisk].Read(block)

	newParity := xor(xor(oldParity, oldData), data)

	r.disks[dataDisk].Write(block, data)
	r.disks[parityDisk].Write(block, newParity)
}

// Read = direct read
func (r *RAID4) Read(blockNum int) []byte {
	dataDisk := blockNum % (NumDisks - 1)
	block := blockNum / (NumDisks - 1)

	return r.disks[dataDisk].Read(block)
}
