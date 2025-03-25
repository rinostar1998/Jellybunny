using System;

namespace Jellybunny
{
    public class Main
    {
        public float var { get; set; }

        public Main()
        {
            Console.WriteLine("Fuck You!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Goodbye World... I've always hated u guys!");
        }

        public void PrintCustardMessage(string mess)
        {
            Console.WriteLine($"Goodbye World... Here's my manifesto : { mess } I've always hated u guys!");
        }
    }
}