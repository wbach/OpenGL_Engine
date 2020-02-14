from NetworkClient import NetworkClient

class Context:
    def __init__(self, root, ip, port):
        self.networkClient = NetworkClient(ip, port)
        self.root = root
        self.screenWidth = self.root.winfo_screenwidth()
        self.screenHeight = self.root.winfo_screenheight()
