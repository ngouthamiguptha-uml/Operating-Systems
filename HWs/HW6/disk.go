package main

import "os"

// Global constants
const NumDisks = 5
const BlockSize = 4096 // 4KB

// Disk represents one file (one physical disk)
type Disk struct {
	file *os.File
}

// Create a new disk file
func NewDisk(name string) *Disk {
	f, _ := os.OpenFile(name, os.O_CREATE|os.O_RDWR, 0644)
	return &Disk{file: f}
}

// Write one block at given block number
func (d *Disk) Write(blockNum int, data []byte) {
	offset := int64(blockNum * BlockSize)
	d.file.WriteAt(data, offset)

	// IMPORTANT: simulate real disk write
	d.file.Sync()
}

// Read one block
func (d *Disk) Read(blockNum int) []byte {
	buf := make([]byte, BlockSize)
	offset := int64(blockNum * BlockSize)
	d.file.ReadAt(buf, offset)
	return buf
}
