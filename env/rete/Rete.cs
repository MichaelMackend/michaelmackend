using System;

namespace rete
{
    class Rete : Command
    {
        public override string Word { get { return "rete"; } }

        private static readonly Rete rete = new Rete();

        static Rete() 
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
