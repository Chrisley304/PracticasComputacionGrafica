
void escribe(void){
	i2c_start();
	i2c_write(0x42);
	i2c_write(dato);
	i2c_stop();
}

void lee(void){
	i2c_start();
	i2c_write(dato);
	var = i2c_read();
	var = i2c_read(0);
	i2c_stop();
}