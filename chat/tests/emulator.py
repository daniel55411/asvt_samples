import serial
import asyncio


PING_TASK = None
COM_PORT = None


def check_serial(handler):
    def wrapper(com_port: serial.Serail, *args):
        assert com_port is not None

        handler(com_port, args)


@check_serial
def handshake(com_port: serial.Serail, *args):
    name: str = args[0]

    com_port.write(f'\\H{len(name)}{name}'.encode())


@check_serial
def who(com_port: serial.Serail, *_):
    com_port.write(b'\\W0')


@check_serial
def message(com_port: serial.Serail, *args):
    msg: str = args[0]

    com_port.write(f'\\M{len(msg)}{msg}'.encode())


@check_serial
def start_ping(com_port: serial.Serail, *_):
    PING_TASK = asyncio.ensure_future(ping(com_port))


def stop_ping(com_port: serial.Serail, *_):
    if PING_TASK is not None:
        PING_TASK.cancel()


def set_uart(_: serial.Serial, *args):
    number = int(args[0])

    with serial.Serial(f'/dev/ttyS{number}') as com_port:
        com_port.baudrate = 125000
        COM_PORT = com_port


async def ping(com_port: serial.Serail, *_):
    try:
        while True:
            com_port.write(b'\\P0')
    except asyncio.CancelledError:
        print('Ping had stopped')
        raise


def main():
    while True:
        args = input('>> ').split()
        
        if args[0] not in dir():
            print(f'Unrecognised command {args[0]}')
            continue
        
        try:
            args[0](COM_PORT, args[1:])
        except AssertionError:
            print('Choose UART before any command')


if __name__ == '__main__':
    pass