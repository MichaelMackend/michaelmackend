using System;

namespace rete
{
    class Program : Command
    {
        public override string Word { get { return "rete"; } }

        private static readonly Program rete = new Program();

        static Program() 
        {
            rete.AddCommand<AppCommand>();
            rete.AddCommand<ServiceCommand>();
        }

        static void Main(string[] args)
        {
            //feed commands one at a time

            Console.WriteLine("GOT ARGS: " + string.Join(',', args));
        }
    }
}
