
class SYSTEM_STATE:
    OK = 1
    ADAMPLUTO_NOT_RDY = 2
    HOST_NOT_RESPONDING = 3
    ADAMPLUTO_RDY = 4
    SYS_NOT_RDY = 5
    NO_INTERNET = 6



class UserValues: 
    STATE = SYSTEM_STATE.SYS_NOT_RDY
    FREQUENCY = 100000
    HOST_ADRESS = "127.0.0.1" # localhost
    HOST_PORT = 32448 #port. Port can be between 1023 to 2^32
    




